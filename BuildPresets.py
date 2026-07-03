#builds all presets and installs 

import argparse
import json
import subprocess
import sys
from pathlib import Path

def run(cmd, cwd):
    print(f"\n>>> {' '.join(cmd)}")
    subprocess.run(cmd, cwd=cwd, check=True)

def load_presets(project_dir):
    user_preset_file = project_dir / "CMakeUserPresets.json"
    preset_file = project_dir / "CMakePresets.json"

    if user_preset_file.exists():
        with user_preset_file.open("r", encoding="utf-8") as f:
            return json.load(f)

    if preset_file.exists():
        with preset_file.open("r", encoding="utf-8") as f:
            return json.load(f)

    raise FileNotFoundError(f"No CMakePresets.json found in {project_dir}")

def collect_builds(data):
    configure_presets = {
        p["name"] for p in data.get("configurePresets", [])
    }

    builds = []

    for build in data.get("buildPresets", []):
        build_name = build["name"]
        configure_name = build.get("configurePreset")

        if configure_name not in configure_presets:
            print(
                f"Skipping '{build_name}' "
                f"(unknown configure preset '{configure_name}')"
            )
            continue

        builds.append((configure_name, build_name))

    builds.sort(key=lambda x: x[1])
    return builds

# def get_binary_dir(data, configure_name, project):
#     for preset in data.get("configurePresets", []):
#         if preset["name"] == configure_name:
#             binary_dir = preset.get("binaryDir", "build")
#             binary_dir = binary_dir.replace("${sourceDir}", str(project))
#             binary_dir = binary_dir.replace("${presetName}", configure_name)
#             return Path(binary_dir)

#     raise ValueError(f"Unknown configure preset {configure_name}")

def main():
    parser = argparse.ArgumentParser(
        description="Configure and build every CMake build preset."
    )

    parser.add_argument(
        "project",
        nargs="?",
        default=".",
        help="Project directory (default: current directory)",
    )

    parser.add_argument(
        "--continue-on-error",
        action="store_true",
        help="Continue building remaining presets after a failure.",
    )

    args = parser.parse_args()

    project = Path(args.project).resolve()

    try:
        presets = load_presets(project)
        builds = collect_builds(presets)
    except Exception as e:
        print(e)
        return 1

    if not builds:
        print("No build presets found.")
        return 0

    print(f"Found {len(builds)} build preset(s).")

    failures = []

    for configure, build in builds:
        print(f"\n=== {configure} -> {build} ===")

        try:
            run(["cmake", "--preset", configure], project)
            run(["cmake", "--build", "--preset", build, "--target", "install"], project)
        except subprocess.CalledProcessError:
            failures.append((configure, build))
            if not args.continue_on_error:
                break

    if failures:
        print("\nThe following builds failed:")
        for configure, build in failures:
            print(f"  {configure} -> {build}")
        return 1

    print("\nAll build presets completed successfully.")
    return 0


if __name__ == "__main__":
    sys.exit(main())