import yaml
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

# Optimal VCO Range (Based on provided example: 8MHz in -> 1000MHz VCO)
VCO_IN_MIN, VCO_IN_MAX = 1000000, 16000000  # 1 MHz - 16 MHz
VCO_OUT_MIN, VCO_OUT_MAX = 150000000, 1000000000  # 150 MHz - 1 GHz

# FDCAN Constraints (Standard / Nominal Bit Timing)
CAN_PRESC_MAX = 512
CAN_TS1_MAX = 256
CAN_TS2_MAX = 128
CAN_MIN_TQ = 8
CAN_MAX_TQ = 80  # Conservative max (Standard allows up to 385 in some modes)


# ==============================================================================
# PLL SOLVER
# ==============================================================================
def solve_pll(source_freq, targets):
    """
    Finds M, N, P, Q, R to match target frequencies (p, q, r).
    Prioritizes exact match for P (System Clock).
    """
    best_solution = None
    min_error_score = float("inf")

    # Iterate reasonable M values to get a valid VCO Input
    for m in range(PLL_M_MIN, PLL_M_MAX + 1):
        vco_in = source_freq / m

        # Constraint: VCO Input Frequency
        if not (VCO_IN_MIN <= vco_in <= VCO_IN_MAX):
            continue

        # Iterate reasonable N values to get a valid VCO Output
        for n in range(PLL_N_MIN, PLL_N_MAX + 1):
            vco_out = vco_in * n

            # Constraint: VCO Output Frequency
            if not (VCO_OUT_MIN <= vco_out <= VCO_OUT_MAX):
                continue

            # Now check dividers P, Q, R
            current_solution = {"m": m, "n": n, "vco_out": vco_out}
            error_score = 0
            valid_pqr = True

            for output_name, target_freq in targets.items():
                if target_freq == 0:
                    current_solution[output_name] = 1  # Dummy value
                    continue

                # Calculate divider: Div = VCO / Target
                # Must be integer
                div = int(round(vco_out / target_freq))

                # Check constraints
                if div < 1 or div > 128:
                    valid_pqr = False
                    break

                actual_freq = vco_out / div
                error = abs(target_freq - actual_freq)

                # Weight P (System Clock) error higher
                weight = 100 if output_name == "p" else 1
                error_score += error * weight

                current_solution[output_name] = div
                current_solution[f"freq_{output_name}"] = actual_freq

            if valid_pqr:
                if error_score < min_error_score:
                    min_error_score = error_score
                    best_solution = current_solution

                # If perfect match, stop early
                if min_error_score == 0:
                    return best_solution

    if best_solution is None:
        raise ValueError(
            f"Could not solve PLL for Source={source_freq} and Targets={targets}"
        )

    return best_solution


# ==============================================================================
# FDCAN SOLVER
# ==============================================================================
def solve_fdcan_timings(kernel_clock, target_baud, sample_point=0.875):
    """
    Calculates Prescaler, TimeSeg1, TimeSeg2 for a given baudrate.
    """
    best_error = 1.0
    best_cfg = None

    # TQ = Prescaler / KernelClock
    # BitTime = TQ * TotalTQ
    # Baud = Kernel / (Presc * TotalTQ)

    # Iterate over possible Prescalers
    for presc in range(1, CAN_PRESC_MAX + 1):
        # Check if this prescaler produces an integer TotalTQ
        # TotalTQ = Kernel / (Presc * Baud)
        total_tq_float = kernel_clock / (presc * target_baud)

        # Check for integer validity (with tolerance for float math)
        if abs(total_tq_float - round(total_tq_float)) > 0.001:
            continue

        total_tq = int(round(total_tq_float))

        if total_tq < CAN_MIN_TQ or total_tq > CAN_MAX_TQ:
            continue

        # Calculate Segments based on Sample Point
        # SamplePoint = (1 + Seg1) / TotalTQ
        # Seg1 = (SamplePoint * TotalTQ) - 1

        seg1 = int(round(sample_point * total_tq)) - 1
        seg2 = total_tq - 1 - seg1

        # Check Segment Limits
        if seg1 < 1 or seg1 > CAN_TS1_MAX:
            continue
        if seg2 < 1 or seg2 > CAN_TS2_MAX:
            continue

        # Calculate actual sample point
        actual_sp = (1 + seg1) / total_tq
        error = abs(actual_sp - sample_point)

        # Prioritize lower prescalers (higher resolution) if error is same
        if error < best_error:
            best_error = error
            best_cfg = {
                "prescaler": presc,
                "ts1": seg1,
                "ts2": seg2,
                "sjw": 1,  # Standard
                "total_tq": total_tq,
                "actual_baud": kernel_clock / (presc * total_tq),
                "actual_sp": actual_sp,
            }
            if error == 0:
                break  # Perfect match found

    return best_cfg


# ==============================================================================
# MAIN LOGIC
# ==============================================================================
def main():
    try:
        with open("config.yaml", "r") as f:
            config = yaml.safe_load(f)
    except FileNotFoundError:
        print("Error: config.yaml not found.")
        return

    print("--- 1. Solving Clock Tree ---")

    # 1. Determine Source Frequency
    clk_cfg = config["clock_config"]
    src_type = "hsi"  # Default
    if clk_cfg["sources"]["hse"].get("enable"):
        src_type = "hse"

    source_freq = clk_cfg["sources"][src_type]["frequency"]
    print(f"Source: {src_type.upper()} @ {source_freq / 1e6:.2f} MHz")

    # 2. Solve PLL1
    if clk_cfg.get("pll1", {}).get("enable"):
        targets = clk_cfg["pll1"]["targets"]
        try:
            pll1_sol = solve_pll(source_freq, targets)
            print(f"\nPLL1 Solution (Source: {source_freq / 1e6} MHz):")
            print(f"  M={pll1_sol['m']}, N={pll1_sol['n']}")
            print(f"  P={pll1_sol['p']} -> {pll1_sol['freq_p'] / 1e6:.2f} MHz (System)")
            print(f"  Q={pll1_sol['q']} -> {pll1_sol['freq_q'] / 1e6:.2f} MHz")
            print(f"  R={pll1_sol['r']} -> {pll1_sol['freq_r'] / 1e6:.2f} MHz")
        except ValueError as e:
            print(f"Error: {e}")
            sys.exit(1)

    # 3. Solve FDCAN Timings
    print("\n--- 2. Solving FDCAN Timings ---")

    # Determine FDCAN Kernel Clock
    # Config says: clock_source: RCC_FDCANCLKSOURCE_PLL2Q (or PLL1Q)
    # We map the YAML string to the solved frequency.
    # NOTE: Assuming config.yaml uses "PLL1Q" or "PLL2Q" logic.

    # For this script, we assume FDCAN uses PLL1_Q or PLL2_Q.
    # Let's check config to see where FDCAN gets its clock.
    # In your config: shared_settings.fdcan_clock_source: RCC_FDCANCLKSOURCE_PLL2Q

    # If PLL2 is not enabled in config, and FDCAN uses PLL2, we have an issue.
    # But usually, if PLL2 is disabled, we fall back to PLL1Q.

    # Let's assume FDCAN uses PLL1_Q for this calculation since PLL2 was disabled in your snippet.
    # If you enable PLL2, you would solve PLL2 similarly to PLL1.

    fdcan_kernel_freq = pll1_sol["freq_q"]  # Taking PLL1_Q as the source
    print(f"FDCAN Kernel Clock: {fdcan_kernel_freq / 1e6:.2f} MHz")

    modules = config.get("modules", {})
    fdcan_group = modules.get("fdcan", {})

    if fdcan_group:
        instances = fdcan_group.get("instances", {})
        for name, inst in instances.items():
            if not inst.get("enable"):
                continue

            baud = inst["speed"]
            try:
                timings = solve_fdcan_timings(fdcan_kernel_freq, baud)

                if timings:
                    print(f"\n  [{name.upper()}] Target: {baud} bps")
                    print(f"    Prescaler: {timings['prescaler']}")
                    print(f"    Seg1:      {timings['ts1']}")
                    print(f"    Seg2:      {timings['ts2']}")
                    print(f"    Sample Pt: {timings['actual_sp'] * 100:.1f}%")
                    print(
                        f"    Error:     {abs(baud - timings['actual_baud']):.2f} bps"
                    )
                else:
                    print(f"  [{name.upper()}] FAILED to find timings for {baud} bps")
            except Exception as e:
                print(f"  Error calculating {name}: {e}")


if __name__ == "__main__":
    main()
