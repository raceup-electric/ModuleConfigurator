import os
import sys
from jinja2 import Environment, FileSystemLoader

import clock_prescaler

#
# ==============================================================================
# 1. MAPPING TABLES & LOOKUPS
# ==============================================================================
# Map simplified strings to C Macros
FDCAN_FILTER_TYPE_MAP = {
    "range": "RUP_FDCAN_FILTER_RANGE",
    "dual": "RUP_FDCAN_FILTER_DUAL",
    "mask": "RUP_FDCAN_FILTER_MASK",
}

FDCAN_FILTER_ACTION_MAP = {
    "fifo0": "RUP_FDCAN_FILTER_TO_RXFIFO0",
    "fifo1": "RUP_FDCAN_FILTER_TO_RXFIFO1",
    "reject": "RUP_FDCAN_FILTER_REJECT",
    "fifo0_hp": "RUP_FDCAN_FILTER_RXFIFO0_HP",
    "fifo1_hp": "RUP_FDCAN_FILTER_RXFIFO1_HP",
}

FDCAN_RXITMODE_MAP = {
    "none": "RUP_FDCAN_IT_NONE",
    "fifo0": "RUP_FDCAN_IT_RX_FIFO0",
    "fifo1": "RUP_FDCAN_IT_RX_FIFO1",
    "all": "RUP_FDCAN_IT_ALL",
}

GPIO_MODE_MAP = {
    "input": "GPIO_MODE_INPUT",
    "output_pp": "GPIO_MODE_OUTPUT_PP",
    "output_od": "GPIO_MODE_OUTPUT_OD",
    "alternate_pp": "GPIO_MODE_AF_PP",
    "alternate_od": "GPIO_MODE_AF_OD",
    "analog": "GPIO_MODE_ANALOG",
    "interrupt_rise": "GPIO_MODE_IT_RISING",
    "interrupt_fall": "GPIO_MODE_IT_FALLING",
    "interrupt_risefall": "GPIO_MODE_IT_RISING_FALLING",
    "event_rise": "GPIO_MODE_EVT_RISING",
    "event_fall": "GPIO_MODE_EVT_FALLING",
    "event_risefall": "GPIO_MODE_EVT_RISING_FALLING",
}

GPIO_SPEED_MAP = {
    "low": "GPIO_SPEED_FREQ_LOW",
    "medium": "GPIO_SPEED_FREQ_MEDIUM",
    "high": "GPIO_SPEED_FREQ_HIGH",
    "very_high": "GPIO_SPEED_FREQ_VERY_HIGH",
}

GPIO_PULL_MAP = {
    "no": "GPIO_NULL",
    "up": "GPIO_PULLUP",
    "down": "GPIO_PULLDOWN",
}

# Database for Alternate Functions (since they are removed from Config)
# Key: (Instance Name, Pin Name) -> Value: AF Macro
AF_DATABASE = {
    # FDCAN1
    ("FDCAN1", "A11"): "GPIO_AF9_FDCAN1",
    ("FDCAN1", "A12"): "GPIO_AF9_FDCAN1",
    ("FDCAN1", "D0"): "GPIO_AF9_FDCAN1",
    ("FDCAN1", "D1"): "GPIO_AF9_FDCAN1",
    ("FDCAN1", "B12"): "GPIO_AF9_FDCAN1",  # Example alternative
    # FDCAN2
    ("FDCAN2", "B12"): "GPIO_AF9_FDCAN2",
    ("FDCAN2", "B13"): "GPIO_AF9_FDCAN2",
    # USART1
    ("USART1", "A9"): "GPIO_AF7_USART1",
    ("USART1", "A10"): "GPIO_AF7_USART1",
}


# ==============================================================================
# 2. PARSING HELPERS (Jinja Filters)
# ==============================================================================
def parse_gpio_bank(pin_str):
    """'A11' -> 'GPIOA'"""
    if not pin_str:
        return "GPIO_NULL"
    bank_char = pin_str[0].upper()
    return f"GPIO{bank_char}"


def parse_gpio_pin(pin_str):
    """'A11' -> 'GPIO_PIN_11'"""
    if not pin_str:
        return "GPIO_PIN_NULL"
    # Extract numbers from the end of the string
    pin_num = "".join(filter(str.isdigit, pin_str))
    return f"GPIO_PIN_{pin_num}"


def resolve_af(pin_str, instance_name):
    """
    Looks up the AF based on Instance + Pin.
    Usage in template: {{ module.pins.rx | resolve_af('FDCAN1') }}
    """
    key = (instance_name.upper(), f"P{pin_str.upper()}")  # Normalize to PA11
    return AF_DATABASE.get(key, f"GPIO_AF_UNKNOWN_{instance_name}_{pin_str}")


def resolve_filter_type(val):
    return FDCAN_FILTER_TYPE_MAP.get(val.lower(), "RUP_FDCAN_FILTER_RANGE")


def resolve_filter_action(val):
    return FDCAN_FILTER_ACTION_MAP.get(val.lower(), "RUP_FDCAN_FILTER_REJECT")


def resolve_gpio_mode(val):
    return GPIO_MODE_MAP.get(val.lower(), "GPIO_MODE_INPUT")


def resolve_gpio_speed(val):
    return GPIO_SPEED_MAP.get(val.lower(), "GPIO_SPEED_FREQ_LOW")


def resolve_gpio_pull(val):
    return GPIO_PULL_MAP.get(val.lower(), "GPIO_NOPULL")


def af_to_hex(af_string):
    """Converts 'afX' to '((uint8_t)0x0X)'."""
    if not af_string.startswith("af"):
        return af_string  # Return as-is if it's not formatted correctly

    af_num = int(af_string[2:])
    return f"((uint8_t)0x{af_num:02X})"


# Path to templates relative to this script
MODULE_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_DIR = os.path.join(MODULE_DIR, "templates")


def resolve_clock_tree(config):
    """
    Delegates clock tree calculation to the external solver module.
    """
    print("\n--- [Phase 2] Resolving Clock Tree ---")

    try:
        # Calls the function we defined in clock_prescaler.py
        config = clock_prescaler.process_config(config)
        print("  [OK] Clock Tree Solved & Timings Calculated.")
    except Exception as e:
        print(f"  [ERR] Clock Solver Failed: {e}")
        sys.exit(1)

    return config


def render_and_write(env, template_name, target_path, config):
    """Helper to render a template and write it to disk."""
    try:
        template = env.get_template(template_name)
        rendered_content = template.render(config=config)

        # Ensure directory exists
        os.makedirs(os.path.dirname(target_path), exist_ok=True)

        with open(target_path, "w") as f:
            f.write(rendered_content)
        print(f"  [GEN] {os.path.basename(target_path)}")

    except Exception as e:
        print(f"  [ERR] Failed to render {template_name}: {e}")


def generate_project_files(config, target_dir):
    """
    Main function to generate dynamic files from templates.
    """
    # 1. Run Solver
    config = resolve_clock_tree(config)

    print("\n--- [Phase 3] Generating Source Code ---")

    # 2. Setup Jinja Environment
    if not os.path.exists(TEMPLATE_DIR):
        print(f"Error: Template directory not found at {TEMPLATE_DIR}")
        sys.exit(1)

    env = Environment(loader=FileSystemLoader(TEMPLATE_DIR))
    env.trim_blocks = True
    env.lstrip_blocks = True

    # REGISTER CUSTOM FILTERS
    env.filters["gpio_bank"] = parse_gpio_bank
    env.filters["gpio_pin"] = parse_gpio_pin
    env.filters["af_lookup"] = resolve_af
    env.filters["filter_type"] = resolve_filter_type
    env.filters["filter_action"] = resolve_filter_action
    env.filters["gpio_mode"] = resolve_gpio_mode
    env.filters["gpio_speed"] = resolve_gpio_speed
    env.filters["gpio_pull"] = resolve_gpio_pull
    env.filters["af_to_hex"] = af_to_hex

    # 3. Define Template Mapping { TemplateFile : OutputPathRelative }
    # You can add more files here easily
    templates_map = {
        "Makefile.jinja": "Makefile",
        "Src/main.c.jinja": "Src/main.c",
        "Src/raceup_setup.c.jinja": "Src/raceup_setup.c",
        "Src/stm32h5xx_it.c.jinja": "Src/stm32h5xx_it.c",
        "Src/syscalls.c": "Src/syscalls.c",
        "Src/sysmem.c": "Src/sysmem.c",
        "Inc/main.h.jinja": "Inc/main.h",
        "Inc/stm32h5xx_hal_conf.h.jinja": "Inc/stm32h5xx_hal_conf.h",
        "Inc/stm32h5xx_it.h": "Inc/stm32h5xx_it.h",
        "Inc/raceup_setup.h.jinja": "Inc/raceup_setup.h",
    }

    # 4. Render All
    for tpl, out in templates_map.items():
        # Check if template exists before trying to render
        if os.path.exists(os.path.join(TEMPLATE_DIR, tpl)):
            full_out_path = os.path.join(target_dir, out)
            render_and_write(env, tpl, full_out_path, config)
        else:
            print(f"  [WARN] Template missing: {tpl}")
