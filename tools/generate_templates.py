import os
import sys
from jinja2 import Environment, FileSystemLoader

# Import the solver (Assumes 'clock_solver.py' exists in the same folder)
try:
    import config_solver as clock_solver
except ImportError:
    clock_solver = None
    print("[WARN] 'config_solver.py' not found. Skipping clock calculations.")

# Path to templates relative to this script
MODULE_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_DIR = os.path.join(MODULE_DIR, "templates")


def resolve_clock_tree(config):
    """
    Delegates clock tree calculation to the external solver module.
    Updates the config object in-place with calculated register values.
    """
    print("\n--- [Phase 2] Resolving Clock Tree ---")

    if clock_solver:
        try:
            # We assume the solver has a main entry point 'process_config'
            # that accepts the config dict and returns the updated dict
            # with calculated timings (prescalers, PLL registers, etc.)
            config = clock_solver.process_config(config)
            print("  [OK] Clock Tree Solved & Timings Calculated.")
        except Exception as e:
            print(f"  [ERR] Clock Solver Failed: {e}")
            sys.exit(1)
    else:
        print("  [SKIP] Mocking resolution (Solver missing).")
        # In a real scenario, you might inject default values here if solver is missing

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

    # 3. Define Template Mapping { TemplateFile : OutputPathRelative }
    # You can add more files here easily
    templates_map = {
        "Makefile.jinja": "Makefile",
        # "Src/main.c.jinja": "Src/main.c",
        # "Src/raceup_msp.c.jinja": "Src/raceup_msp.c",
        "Src/stm32h5xx_it.c.jinja": "Src/stm32h5xx_it.c",
        # "Inc/main.h": "Inc/main.h",  # Static or Jinja
        "Inc/stm32h5xx_hal_conf.h.jinja": "Inc/stm32h5xx_hal_conf.h",  # Static or Jinja
    }

    # 4. Render All
    for tpl, out in templates_map.items():
        # Check if template exists before trying to render
        if os.path.exists(os.path.join(TEMPLATE_DIR, tpl)):
            full_out_path = os.path.join(target_dir, out)
            render_and_write(env, tpl, full_out_path, config)
        else:
            print(f"  [WARN] Template missing: {tpl}")
