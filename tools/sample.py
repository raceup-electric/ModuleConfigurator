import yaml
import json


def read_and_print_config(filename="config.yaml"):
    try:
        # 1. Open and load the YAML file
        with open(filename, "r") as f:
            config_data = yaml.safe_load(f)

        if config_data is None:
            print(f"The file '{filename}' is empty.")
            return

        # 2. Print as a Prettified JSON string (often easiest to read for structure)
        print("--- Configuration Structure (JSON Style) ---")
        print(json.dumps(config_data, indent=4))

        # 3. (Optional) Print re-dumped as clean YAML
        print("\n--- Configuration Re-Dumped (YAML Style) ---")
        print(yaml.dump(config_data, sort_keys=False, default_flow_style=False))

    except FileNotFoundError:
        print(
            f"Error: Could not find file '{filename}'. Make sure it is in the same directory."
        )
    except yaml.YAMLError as exc:
        print(f"Error parsing YAML file: {exc}")


if __name__ == "__main__":
    read_and_print_config()
