import os
import subprocess
import argparse
import shutil
from pathlib import Path
import logging

# pip install psycopg2 clustershell tabulate

def do_user_install(phobos_python_dir: Path, phobos_install_dir: Path):

    temp_dir = Path("/tmp/phobos_test/")
    if temp_dir.exists():
        shutil.rmtree(temp_dir)

    temp_dir.mkdir(755)
    shutil.copytree(phobos_python_dir, temp_dir / phobos_python_dir.name)
    shutil.copytree(phobos_install_dir, temp_dir / phobos_install_dir.name)

    os.system(f'chmod -R 755 {temp_dir}')

    lib_path = f"{temp_dir / phobos_install_dir.name}/lib"
    python_path = str(temp_dir / phobos_python_dir.name)
    return lib_path, python_path

def setup_db(phobos_python_dir: Path, phobos_install_dir: Path):
    if phobos_python_dir is not None:
        lib_path, python_path = do_user_install(phobos_python_dir, phobos_install_dir)
        prefix = f"export PYTHONPATH={python_path}; export LD_LIBRARY_PATH={lib_path}; "
    else:
        prefix = ""

    cmd = prefix + "python3 -m phobos.db setup_db -s -p phobos"
    logging.info("Launching:" + cmd)

    subprocess.run(cmd, shell=True, cwd=os.getcwd(), user="postgres")

if __name__ == "__main__":

    logging.basicConfig(level=logging.INFO)

    parser = argparse.ArgumentParser()
    parser.add_argument('--phobos_source_dir', type=Path, default=None)
    parser.add_argument('--phobos_install_dir', type=Path, default=None)

    args = parser.parse_args()

    if args.phobos_source_dir is not None:
        python_version = "3.10"
        arch = "x86_64"
        phobos_python_dir = f"{args.phobos_source_dir}/src/cli/build/lib.linux-{arch}-{python_version}"
    else:
        phobos_python_dir = None

    setup_db(Path(phobos_python_dir), args.phobos_install_dir)