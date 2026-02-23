# STM32H5 FreeRTOS Module Configurator

This repository provides an automated, template-driven build system and firmware architecture for the **STM32H563VIT6x** microcontroller. It leverages Python and Jinja2 to parse a central `config.yaml` file, automatically generating the low-level C code required to initialize hardware peripherals (like FDCAN and GPIO) and FreeRTOS tasks.

## 🚀 Features

* **Centralized Configuration:** Define your FreeRTOS settings, CAN bitrates, filters, interrupts, and GPIO pins in a single, human-readable `config.yaml` file.
* **Automated Code Generation:** Uses Jinja2 templates to generate standard, bug-free C setup code (`raceup_setup.c`), bridging the gap between high-level configuration and STM32 HAL.
* **Modern C++ / C Hybrid:** Application logic is written in C++ (`app/main_app.cpp`), while low-level hardware initialization remains in C.
* **Pre-configured Build System:** Uses CMake and Ninja with pre-defined presets for streamlined building.

---

## 🛠 Prerequisites

Ensure you have the following tools installed and available in your system's PATH:

1. **Python 3.8+** (for code generation)
   * Install required packages: `pip install pyyaml jinja2`
2. **CMake 3.20+** (Build system generator)
3. **Ninja** (Build tool)
4. **ARM GNU Toolchain** (`arm-none-eabi-gcc`, `arm-none-eabi-g++`)

---

## 📂 Project Structure

```text
.
├── CMakeLists.txt              # Root CMake configuration (Hardcoded for STM32H563VIT6x)
├── CMakePresets.json           # Build/Configure presets for CMake
├── config.yaml                 # 🌟 Central hardware/OS configuration file
├── generate.py                 # 🐍 Python script that renders Jinja templates
├── app/
│   └── main_app.cpp            # Application entry point (app_start) and FreeRTOS tasks
├── instances/
│   └── stm32/stm32h5xx/        # Core STM32 family files (main.cpp, HAL MSP, ISRs)
└── lib/
    └── drivers/
        ├── include/            # C/C++ Header files (raceup_fdcan.h, etc.)
        └── instances/stm32h5xx/
            ├── templates/
            │   └── raceup_setup.c.j2  # Jinja template for hardware setup
            ├── raceup_setup.c         # ⚠️ AUTO-GENERATED output file
            └── raceup_fdcan.c         # Custom FDCAN wrapper implementation

```

---

## ⚙️ Workflow & Configuration

### 1. Edit `config.yaml`

All peripheral configurations live inside `config.yaml`.

* **OS Config:** Set heap size, tick rate, and automatically define tasks.
* **FDCAN Modules:** Enable instances, set RX/TX pins, configure NVIC priorities, and define global/specific reception filters (Range, Dual, Mask).
* **GPIO Modules:** Define LEDs, output/input modes, and speeds.

### 2. Generate the Setup Code

Before building, you must run the Python generator to translate `config.yaml` into C code. From the root of the repository, run:

```bash
python generate.py

```

This script reads `config.yaml` and the template at `lib/drivers/instances/stm32h5xx/templates/raceup_setup.c.j2`, outputting the highly-specific initialization code to `lib/drivers/instances/stm32h5xx/raceup_setup.c`.

*(Note: Do not edit `raceup_setup.c` directly, as your changes will be overwritten the next time `generate.py` runs. Make all changes in the `.j2` template or `config.yaml`!)*

### 3. Application Logic

Once the hardware is configured via the YAML file, write your high-level application logic in `app/main_app.cpp`. The generated code automatically handles HAL initialization, clocks, and peripheral setup before handing control over to `app_start()`.

---

## 🔨 Building the Firmware

This project uses `CMakePresets.json` to simplify the build process for the `stm32h563vit6x` target.

**To configure the project:**

```bash
cmake --preset stm32h563vit6x

```

**To build the project:**

```bash
cmake --build --preset stm32h563vit6x

```

**To configure and build in one step:**

```bash
cmake --workflow --preset stm32h563vit6x

```

The compiled binary (`firmware.elf`) will be located in the `.build-stm32h563vit6x/` directory.

---

## 📝 Extending the Templates

To add support for a new peripheral (e.g., UART, SPI):

1. Add a new configuration block for the module in `config.yaml`.
2. Write the initialization logic inside `lib/drivers/instances/stm32h5xx/templates/raceup_setup.c.j2` using Jinja2 syntax to iterate over your new YAML block.
3. Expose the generated init function (e.g., `void config_UART(void);`) via `extern "C"` in `instances/stm32/stm32h5xx/Src/main.cpp` and call it before `app_start()`.
