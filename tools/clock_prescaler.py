import sys

# ==============================================================================
# CONSTANTS & CONSTRAINTS (STM32H5)
# ==============================================================================
# PLL Constraints
PLL_M_MIN, PLL_M_MAX = 1, 63
PLL_N_MIN, PLL_N_MAX = 4, 512
PLL_P_MIN, PLL_P_MAX = 1, 128
PLL_Q_MIN, PLL_Q_MAX = 1, 128
PLL_R_MIN, PLL_R_MAX = 1, 128

# Optimal VCO Range (Ref Manual RM0481)
VCO_IN_MIN, VCO_IN_MAX = 1000000, 16000000  # 1 MHz - 16 MHz
VCO_OUT_MIN, VCO_OUT_MAX = 150000000, 1000000000  # 150 MHz - 1 GHz

# FDCAN Constraints (Standard / Nominal Bit Timing)
CAN_PRESC_MAX = 512
CAN_TS1_MAX = 256
CAN_TS2_MAX = 128
CAN_MIN_TQ = 8
CAN_MAX_TQ = 80


# ==============================================================================
# PLL SOLVER
# ==============================================================================
def solve_pll(source_freq, targets):
    """
    Finds M, N, P, Q, R to match target frequencies.
    Returns a dict with registers (m, n, p, q, r) and actual frequencies.
    """
    best_solution = None
    min_error_score = float("inf")

    # 1. Iterate M to get valid VCO Input
    for m in range(PLL_M_MIN, PLL_M_MAX + 1):
        vco_in = source_freq / m
        if not (VCO_IN_MIN <= vco_in <= VCO_IN_MAX):
            continue

        # 2. Iterate N to get valid VCO Output
        for n in range(PLL_N_MIN, PLL_N_MAX + 1):
            vco_out = vco_in * n
            if not (VCO_OUT_MIN <= vco_out <= VCO_OUT_MAX):
                continue

            current_solution = {"regs": {"m": m, "n": n}, "freqs": {"vco_out": vco_out}}

            error_score = 0
            valid_pqr = True

            # 3. Calculate Dividers P, Q, R
            for output_name, target_freq in targets.items():
                if not target_freq or target_freq == 0:
                    current_solution["regs"][output_name] = 1  # Dummy valid value
                    current_solution["freqs"][output_name] = 0
                    continue

                div = int(round(vco_out / target_freq))

                # Check divider limits
                if div < 1 or div > 128:
                    valid_pqr = False
                    break

                actual_freq = vco_out / div
                error = abs(target_freq - actual_freq)

                # Heavily penalize P (System Clock) errors
                weight = 100 if output_name == "p" else 1
                error_score += error * weight

                current_solution["regs"][output_name] = div
                current_solution["freqs"][output_name] = actual_freq

            if valid_pqr:
                if error_score < min_error_score:
                    min_error_score = error_score
                    best_solution = current_solution

                if min_error_score == 0:
                    return best_solution

    if best_solution is None:
        raise ValueError(
            f"Could not solve PLL for Source={source_freq} and Targets={targets}"
        )

    return best_solution


# ==============================================================================
# FDCAN SOLVER (Improved to mimic can-wiki / Kvaser logic)
# ==============================================================================
def solve_fdcan_timings(kernel_clock, target_baud):
    """
    Calculates Prescaler, TimeSeg1, TimeSeg2 for a given baudrate.
    Iterates through all valid prescalers to find the best Sample Point match.
    """
    # STM32H5 FDCAN Nominal Bit Timing Constraints
    # Prescaler: 1 to 512
    # TimeSeg1: 1 to 256
    # TimeSeg2: 1 to 128
    # Total TQ: 1 (Sync) + TimeSeg1 + TimeSeg2

    MIN_TQ = 8
    MAX_TQ = 385  # 1 + 256 + 128
    TARGET_SP = 0.875  # Target 87.5%

    best_error = 1.0
    best_cfg = None

    print(f"       [SOLVER] Solving {target_baud}bps @ {kernel_clock / 1e6:.2f}MHz")

    # Iterate ALL Prescalers (not just 1)
    for presc in range(1, 512 + 1):
        # Calculate ideal Total Time Quanta for this prescaler
        # Baud = Clock / (Presc * TotalTQ) -> TotalTQ = Clock / (Presc * Baud)
        tq_float = kernel_clock / (presc * target_baud)
        tq_total = int(round(tq_float))

        # 1. Check if the clock divides cleanly (Integer Match)
        if abs(tq_float - tq_total) > 0.01:
            continue

        # 2. Check hardware limits for Total TQ
        if not (MIN_TQ <= tq_total <= MAX_TQ):
            continue

        # 3. Calculate Segments for Target Sample Point
        # Seg1 = (TotalTQ * SP) - 1 (SyncSeg)
        seg1 = int(round(tq_total * TARGET_SP)) - 1
        seg2 = tq_total - 1 - seg1

        # 4. Check Segment limits
        if (seg1 < 1 or seg1 > 256) or (seg2 < 1 or seg2 > 128):
            continue

        # 5. Calculate Errors
        actual_sp = (1 + seg1) / tq_total
        sp_error = abs(TARGET_SP - actual_sp)

        # 6. Selection Logic:
        #    Strictly minimize Sample Point error.
        #    If equal error, the loop order (low prescaler first) preserves
        #    the higher resolution solution (which is technically superior).
        if sp_error < best_error:
            best_error = sp_error
            best_cfg = {
                "prescaler": presc,
                "ts1": seg1,
                "ts2": seg2,
                "sjw": max(1, min(seg2, 128)),  # SJW <= Seg2
                "tq_total": tq_total,  # Stored for printing
                "actual_sp": actual_sp,
                "actual_baud": kernel_clock / (presc * tq_total),
            }

            # If we hit the target exactly, we can technically stop or keep
            # searching if you really preferred higher prescalers.
            # For now, we take the first exact match (Highest Res).
            if best_error == 0.0:
                break

    if best_cfg:
        # Requested Printout
        print(
            f"         -> Found Solution: Prescaler={best_cfg['prescaler']}, "
            f"TimeQuanta={best_cfg['tq_total']}, "
            f"SamplePoint={best_cfg['actual_sp'] * 100:.2f}%"
        )
        return best_cfg

    print(f"       [ERR] No valid FDCAN timings found for {target_baud}")
    return None


# ==============================================================================
# MAIN PROCESSING
# ==============================================================================
def process_config(config):
    """
    Main entry point called by generate_templates.py.
    Updates the config dict in-place with calculated values.
    """
    print("  [SOLVER] Starting Clock Tree Resolution...")

    clk_cfg = config.get("clock_config", {})
    sources = clk_cfg.get("sources", {})

    # 1. Solve PLLs
    # --------------------------------------------------------------------------
    # Dictionary to hold available frequencies for peripheral lookup later
    available_clocks = {}

    for pll_name in ["pll1", "pll2"]:
        pll_cfg = clk_cfg.get(pll_name)
        if not pll_cfg:
            continue

        # Even if enabled is False, we solve if targets exist (incase user toggles it later)
        targets = pll_cfg.get("targets", {})
        has_targets = any(v > 0 for v in targets.values())

        if not has_targets:
            continue

        # Determine Input Source Frequency
        src_name = pll_cfg.get("source", "hsi").lower()
        if src_name not in sources:
            print(
                f"  [WARN] {pll_name} source '{src_name}' not found. Defaulting to HSI."
            )
            src_name = "hsi"

        src_freq = sources[src_name]["frequency"]

        try:
            solution = solve_pll(src_freq, targets)

            # Inject results into config
            # Usage in Jinja: config.clock_config.pll1.regs.m
            pll_cfg["regs"] = solution["regs"]
            pll_cfg["calculated_freqs"] = solution["freqs"]

            # MODIFIED PRINT STATEMENT
            regs = solution["regs"]
            print(
                f"    -> Solved {pll_name.upper()}: M={regs['m']}, N={regs['n']}, "
                f"P={regs.get('p', '-')}, Q={regs.get('q', '-')}, R={regs.get('r', '-')}"
            )

            # Store for peripheral lookup
            available_clocks[f"{pll_name}p"] = solution["freqs"].get("p", 0)
            available_clocks[f"{pll_name}q"] = solution["freqs"].get("q", 0)
            available_clocks[f"{pll_name}r"] = solution["freqs"].get("r", 0)

        except ValueError as e:
            print(f"    [ERR] {e}")
            sys.exit(1)

    # 2. Solve FDCAN Timings
    # --------------------------------------------------------------------------
    modules = config.get("modules", {})
    fdcan_group = modules.get("fdcan", {})

    if fdcan_group and fdcan_group.get("enable"):
        # Determine Kernel Clock
        clk_source_str = fdcan_group.get("clock_source", "").lower().replace("_", "")
        kernel_freq = available_clocks.get(clk_source_str, 0)

        if kernel_freq == 0:
            print(
                f"    [WARN] FDCAN source '{clk_source_str}' has 0Hz or is invalid. Skipping FDCAN calc."
            )
        else:
            print(
                f"    -> FDCAN Kernel Clock: {kernel_freq / 1e6:.2f} MHz (Source: {clk_source_str})"
            )

            instances = fdcan_group.get("instances", {})
            for inst_name, inst in instances.items():
                if not inst.get("enable"):
                    continue

                baud = inst.get("speed", 500000)
                timings = solve_fdcan_timings(kernel_freq, baud)

                if timings:
                    # Inject results into config
                    # Usage in Jinja: config.modules.fdcan.instances.fdcan1.timings.prescaler
                    inst["timings"] = timings
                    print(
                        f"       [{inst_name}] {baud}bps -> Presc: {timings['prescaler']}, Seg1: {timings['ts1']}, Seg2: {timings['ts2']}"
                    )
                else:
                    print(
                        f"       [ERR] Failed to find FDCAN timings for {inst_name} @ {baud}"
                    )

    return config


if __name__ == "__main__":
    # Test stub for standalone execution
    import yaml

    try:
        with open("config.yaml", "r") as f:
            cfg = yaml.safe_load(f)
            process_config(cfg)
    except FileNotFoundError:
        print("config.yaml not found for testing.")
