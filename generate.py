import yaml
import os
from jinja2 import Environment, FileSystemLoader


# Custom Jinja filters to extract the Bank (e.g., 'D' from 'D0') and Pin (e.g., '0' from 'D0')
def pinbank(pin_str):
    return pin_str[0].upper()


def pinno(pin_str):
    return pin_str[1:]


def main():
    # 1. Load the YAML configuration from the root folder
    with open("config.yaml", "r") as f:
        config = yaml.safe_load(f)

    # Define paths
    template_dir = "./lib/drivers/instances/stm32h5xx/templates"
    template_name = "raceup_setup.c.j2"
    output_file = "./lib/drivers/instances/stm32h5xx/raceup_setup.c"

    # 2. Setup Jinja2 environment pointing to the specific templates directory
    env = Environment(loader=FileSystemLoader(template_dir))
    env.filters["pinbank"] = pinbank
    env.filters["pinno"] = pinno

    # Load the template
    template = env.get_template(template_name)

    # 3. Render the template with the config data
    rendered_code = template.render(config)

    # 4. Write the output to the target C file
    # Ensure the target directory exists before writing
    os.makedirs(os.path.dirname(output_file), exist_ok=True)

    with open(output_file, "w") as f:
        f.write(rendered_code)

    print(f"Successfully generated {output_file}")


if __name__ == "__main__":
    main()
