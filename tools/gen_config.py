import os
import shutil
import yaml
from jinja2 import Environment, FileSystemLoader

# --- Configuration ---
RESOURCE_DIR = "resources"
TEMPLATE_DIR = "templates"
DRIVER_DIR = "drivers"
CONFIG_FILE = "project.yaml"


def generate_project():
    # 1. Load Config
    with open(CONFIG_FILE, "r") as f:
        config = yaml.safe_load(f)

    output_dir = config["target_dir"]
    print(f"Generating project: {config['project_name']} in {output_dir}...")

    # 2. Prepare Output Directory (Clean Slate)
    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(f"{output_dir}/Src")
    os.makedirs(f"{output_dir}/Inc")

    # 3. Copy Static Resources (HAL, CMSIS, Startup, Linker)
    print("  - Copying Drivers and Startup files...")
    shutil.copytree(f"{RESOURCE_DIR}/Drivers", f"{output_dir}/Drivers")
    shutil.copytree(f"{RESOURCE_DIR}/Startup", f"{output_dir}/Startup")
    shutil.copy(
        f"{RESOURCE_DIR}/Linker/STM32H563xx_FLASH.ld",
        f"{output_dir}/STM32H563xx_FLASH.ld",
    )

    # 4. Copy RaceUp Drivers (Only enabled ones)
    print("  - Injecting RaceUp Drivers...")
    # Always copy headers to Inc, source to Src if enabled
    drivers_to_copy = []
    if config["modules"].get("fdcan1", {}).get("enable"):
        drivers_to_copy.append("raceup_fdcan")
    if config["modules"].get("usart1", {}).get("enable"):
        drivers_to_copy.append("raceup_usart")

    for drv in drivers_to_copy:
        shutil.copy(f"{DRIVER_DIR}/{drv}.c", f"{output_dir}/Src/")
        shutil.copy(f"{DRIVER_DIR}/{drv}.h", f"{output_dir}/Inc/")

    # 5. Template Generation (The Magic)
    env = Environment(loader=FileSystemLoader(TEMPLATE_DIR))

    templates = {
        "Makefile.jinja": "Makefile",
        "main.c.jinja": "Src/main.c",
        "raceup_msp.c.jinja": "Src/raceup_msp.c",
        "stm32h5xx_it.c.jinja": "Src/stm32h5xx_it.c",
        "stm32h5xx_hal_conf.h": "Inc/stm32h5xx_hal_conf.h",  # Static config
    }

    print("  - Rendering Templates...")
    for tpl_name, out_name in templates.items():
        if tpl_name.endswith(".jinja"):
            template = env.get_template(tpl_name)
            rendered = template.render(config=config, drivers=drivers_to_copy)
            with open(f"{output_dir}/{out_name}", "w") as f:
                f.write(rendered)
        else:
            # Just copy static templates (like hal_conf)
            shutil.copy(f"{TEMPLATE_DIR}/{tpl_name}", f"{output_dir}/{out_name}")

    print("Done! Ready to compile.")


if __name__ == "__main__":
    generate_project()
