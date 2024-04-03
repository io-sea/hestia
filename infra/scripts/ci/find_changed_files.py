#!/usr/bin/env python3

import subprocess
from pathlib import Path


def find_changed_files(extensions):
    op = "git diff --name-only"
    output = subprocess.run(op, shell=True, check=True,
                            capture_output=True, text=True)
    files = output.stdout.splitlines()
    filtered_files = [f for f in files if Path(f).suffix in extensions]
    for file in filtered_files:
        print(file)


if __name__ == "__main__":
    extensions = [".h", ".cc"]
    find_changed_files(extensions)
