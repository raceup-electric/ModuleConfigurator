import os
import shutil

# ==============================================================================
# CONSTANTS & PATHS
# ==============================================================================
# Determine path relative to THIS file (so it works when imported)
MODULE_DIR = os.path.dirname(os.path.abspath(__file__))
RESOURCES_DIR = os.path.join(MODULE_DIR, "..", "resources")

# Resource Sub-paths
RES_MAP = {
    "HAL": os.path.join(RESOURCES_DIR, "STM32H5xx_HAL_Driver"),
    "CMSIS_DEVICE": os.path.join(RESOURCES_DIR, "CMSIS", "Device", "ST", "STM32H5xx"),
    "CMSIS_CORE": os.path.join(RESOURCES_DIR, "CMSIS_Core", "CMSIS", "Core", "Include"),
    "FDCAN": os.path.join(RESOURCES_DIR, "FDCANDriver"),
    "USART": os.path.join(RESOURCES_DIR, "USARTDriver"),
}


def safe_copy_file(src, dst_folder):
    """Copies a single file to a destination folder, creating the folder if needed."""
    if not os.path.exists(src):
        print(f"  [ERR] Missing Source: {os.path.relpath(src, RESOURCES_DIR)}")
        return

    os.makedirs(dst_folder, exist_ok=True)
    shutil.copy2(src, dst_folder)
    print(f"  [CPY] {os.path.relpath(src, RESOURCES_DIR)}")


def safe_copy_tree(src_dir, dst_dir, ext_filter=None):
    """Copies entire directory content, optionally filtering by extension."""
    if not os.path.exists(src_dir):
        print(f"  [ERR] Missing Dir: {os.path.relpath(src_dir, RESOURCES_DIR)}")
        return

    os.makedirs(dst_dir, exist_ok=True)

    for item in os.listdir(src_dir):
        s = os.path.join(src_dir, item)
        d = os.path.join(dst_dir, item)

        if os.path.isfile(s):
            if ext_filter and not item.endswith(ext_filter):
                continue
            shutil.copy2(s, d)

    print(f"  [CPY] {os.path.relpath(src_dir, RESOURCES_DIR)}")


def copy_project_files(config, target_dir):
    """
    Main function to copy all static resources to the target directory.
    """
    print(f"\n--- [Phase 1] Copying Static Resources ---")

    # 1. Copy CMSIS (Core & Device)
    dst_core = os.path.join(target_dir, "Drivers", "CMSIS", "Include")
    safe_copy_tree(RES_MAP["CMSIS_CORE"], dst_core, ".h")

    dst_dev = os.path.join(
        target_dir, "Drivers", "CMSIS", "Device", "ST", "STM32H5xx", "Include"
    )
    src_dev = os.path.join(RES_MAP["CMSIS_DEVICE"], "Include")
    safe_copy_tree(src_dev, dst_dev, ".h")

    # 2. Copy HAL Drivers
    dst_hal = os.path.join(target_dir, "Drivers", "STM32H5xx_HAL_Driver")
    safe_copy_tree(
        os.path.join(RES_MAP["HAL"], "Inc"), os.path.join(dst_hal, "Inc"), ".h"
    )
    safe_copy_tree(
        os.path.join(RES_MAP["HAL"], "Inc", "Legacy"),
        os.path.join(dst_hal, "Inc", "Legacy"),
        ".h",
    )
    safe_copy_tree(
        os.path.join(RES_MAP["HAL"], "Src"), os.path.join(dst_hal, "Src"), ".c"
    )

    # 3. Copy Startup & Linker
    # /home/lucadomene/Documents/stm32h5/ModuleConfigurator/resources/CMSIS/Device/ST/STM32H5xx/Source/Templates/gcc
    family = config.get("mcu", {}).get("family", "STM32H5xx")
    part_number = config.get("mcu", {}).get("part_number", "STM32H563xx")

    startup_file = f"startup_{part_number.lower()}.s"
    src_startup = os.path.join(
        RES_MAP["CMSIS_DEVICE"], "Source", "Templates", "gcc", startup_file
    )
    dst_startup = os.path.join(target_dir, "Startup")
    safe_copy_file(src_startup, dst_startup)

    linker_file = f"{part_number}_FLASH.ld"
    src_linker = os.path.join(
        RES_MAP["CMSIS_DEVICE"], "Source", "Templates", "gcc", "linker", linker_file
    )
    safe_copy_file(src_linker, target_dir)

    system_file = f"system_{family.lower()}.c"
    src_system = os.path.join(
        RES_MAP["CMSIS_DEVICE"], "Source", "Templates", system_file
    )
    dst_system = os.path.join(target_dir, "Src")
    safe_copy_file(src_system, dst_system)

    # 4. Copy Enabled Modules
    modules = config.get("modules", {})

    # --- FDCAN ---
    fdcan_enabled = modules.get("fdcan", {}).get("enable", False)
    if not fdcan_enabled:
        for inst in modules.get("fdcan", {}).get("instances", {}).values():
            if inst.get("enable"):
                fdcan_enabled = True
                break

    if fdcan_enabled:
        safe_copy_file(
            os.path.join(RES_MAP["FDCAN"], "Inc", "raceup_fdcan.h"),
            os.path.join(target_dir, "Inc"),
        )
        safe_copy_file(
            os.path.join(RES_MAP["FDCAN"], "Src", "raceup_fdcan.c"),
            os.path.join(target_dir, "Src"),
        )

    # --- USART ---
    usart_enabled = modules.get("usart", {}).get("enable", False)
    if not usart_enabled:
        for inst in modules.get("usart", {}).get("instances", {}).values():
            if inst.get("enable"):
                usart_enabled = True
                break

    if usart_enabled:
        safe_copy_file(
            os.path.join(RES_MAP["USART"], "Inc", "raceup_usart.h"),
            os.path.join(target_dir, "Inc"),
        )
        safe_copy_file(
            os.path.join(RES_MAP["USART"], "Src", "raceup_usart.c"),
            os.path.join(target_dir, "Src"),
        )
