import os
import yaml
from pathlib import Path
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

    # Define the base directory to search for templates (adjust as needed)
    base_dir = Path(".")

    # 2. Find all .j2 template files recursively in the base directory
    for template_path in base_dir.rglob("*.j2"):
        # Extract folder and file names
        template_dir = template_path.parent
        template_name = template_path.name

        # 3. Calculate the target output path
        # If the template is inside a "templates" folder, place the output in the parent directory
        if template_dir.name == "templates":
            output_dir = template_dir.parent
        else:
            output_dir = template_dir  # Fallback: keep it in the same directory

        # Remove the '.j2' extension (e.g., raceup_setup.c.j2 -> raceup_setup.c)
        output_filename = template_path.with_suffix("").name
        output_file = output_dir / output_filename

        # 4. Setup Jinja2 environment for the current template's specific directory
        env = Environment(loader=FileSystemLoader(template_dir))
        env.filters["pinbank"] = pinbank
        env.filters["pinno"] = pinno

        # Load the template
        template = env.get_template(template_name)

        # 5. Render the template with the config data
        rendered_code = template.render(config)

        # 6. Write the output to the target file
        # Ensure the target directory exists before writing
        output_file.parent.mkdir(parents=True, exist_ok=True)

        with open(output_file, "w") as f:
            f.write(rendered_code)

        print(f"Successfully generated {output_file}")


if __name__ == "__main__":
    main()
