import os
import sys
import yaml
import shutil

# Import our custom modules
import tools.copy_files as copy_files
import tools.generate_templates as generate_templates

# ==============================================================================
# CONFIGURATION
# ==============================================================================
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
CONFIG_FILE = os.path.join(SCRIPT_DIR, "config.yaml")


def load_config(path):
    if not os.path.exists(path):
        print(f"Error: Configuration file not found at {path}")
        sys.exit(1)
    with open(path, "r") as f:
        return yaml.safe_load(f)


def main():
    print("==================================================")
    print("      RaceUp STM32 Project Generator v1.0         ")
    print("==================================================")

    # 1. Load Configuration
    config = load_config(CONFIG_FILE)
    project_name = config.get("project_name", "Untitled_Project")

    # 2. Determine Output Directory
    # Output is placed one level above the 'tools' directory
    output_root = os.path.abspath(os.path.join(SCRIPT_DIR, ".."))
    project_path = os.path.join(output_root, project_name)

    print(f"Project Name:   {project_name}")
    print(f"Output Path:    {project_path}")

    # 3. Setup Workspace (Clean/Create)
    if os.path.exists(project_path):
        print(f"\n[WARN] Target directory '{project_name}' already exists.")
        ack = input("      Overwrite? (y/n): ")
        if ack.lower() != "y":
            print("Aborted.")
            sys.exit(0)
        shutil.rmtree(project_path)
    os.makedirs(project_path)

    # 4. Execute Modules
    try:
        # Step A: Copy Static Files (HAL, Drivers, Startup)
        copy_files.copy_project_files(config, project_path)

        # Step B: Solve Clocks & Generate Dynamic Files (Makefile, Main, MSP)
        generate_templates.generate_project_files(config, project_path)

    except Exception as e:
        print(f"\n[FATAL ERROR] {e}")
        sys.exit(1)

    print("\n==================================================")
    print(f"SUCCESS! Project generated at: {project_path}")
    print("To build:")
    print(f"  cd {project_path}")
    print("  make")
    print("==================================================")


if __name__ == "__main__":
    main()
