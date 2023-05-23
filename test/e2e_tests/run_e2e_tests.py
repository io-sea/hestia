import os
import sys
import argparse
import logging
import shutil
from pathlib import Path
import tarfile
import subprocess
import filecmp

def install_package(e2e_dir: Path, package_path: Path, project_name: str = "hestia") -> bool:
    
    system_install = False
    if str(package_path).endswith(".tar.gz"):
        logging.info(f"Extracting: {package_path} to {e2e_dir}")
        tar = tarfile.open(package_path)
        tar.extractall(e2e_dir)
        tar.close()

        package_name = package_path.stem

        # Remove .tar from package name
        components = package_path.stem.split(".")
        package_name = package_name[0:-4]

        extracted_path = e2e_dir / package_name
        extracted_path.rename(e2e_dir / project_name)

    elif str(package_path).endswith(".rpm"):
        logging.info(f"Installing RPM: {package_path}")

        cmd = f"yum install -y {package_path}"
        subprocess.run(cmd, shell=True)
        logging.info(f"RPM installed")
        system_install = True
    return system_install

def run_ops(runtime_path: Path, runtime_env, ops: list):
    for op in ops:
        logging.info("Launching: " + op)
        subprocess.run(op, shell=True, cwd=runtime_path, env=runtime_env)
        logging.info("Finished CLI command")

def run_cli_tests(e2e_dir: Path):

    logging.info("Running CLI tests")

    object_content = e2e_dir / "test_data" / "EmperorWu.txt"
    return_cache = e2e_dir / "object.dat"

    runtime_path = e2e_dir / "cli_tests"
    Path.mkdir(runtime_path)

    runtime_env = os.environ.copy()
    project_bin_dir = e2e_dir / "hestia" / "bin"
    project_lib_dir = e2e_dir / "hestia" / "lib"
    runtime_env["PATH"] = f"{project_bin_dir}:{runtime_env['PATH']}"

    if "LD_LIBRARY_PATH" in runtime_env:
        runtime_env["LD_LIBRARY_PATH"] = f"{project_lib_dir}:{runtime_env['LD_LIBRARY_PATH']}"
    else:
        runtime_env["LD_LIBRARY_PATH"] = f"{project_lib_dir}"
    
    put_cmd = f"hestia put 00001234 {object_content} 0"
    get_cmd = f"hestia get 00001234 {return_cache} 0"

    ops = [put_cmd, get_cmd]
    run_ops(runtime_path, runtime_env, ops)

    same = filecmp.cmp(object_content, return_cache)
    if not same:
        raise ValueError(f'Object content mismatch after put-get')
    
def run_cmake_sample_app(e2e_dir: Path, system_install: bool):

    logging.info("Running CMake sample app")

    runtime_path = e2e_dir / "sample_cmake_app"
    build_dir = runtime_path / "build"

    if not system_install:
        hestia_dir = e2e_dir / "hestia"
        cmake_args = f"-Dhestia_DIR={hestia_dir}/lib/cmake/hestia "
    else:
        cmake_args = ""

    cmd = f"mkdir {build_dir}; cd {build_dir}; cmake {cmake_args}../; make; ./hestia_sample_app"
    subprocess.run(cmd, shell=True, cwd=runtime_path)

def run_autotools_sample_app(e2e_dir: Path, system_install: bool):

    logging.info("Running Autotools sample app")

    runtime_path = e2e_dir / "sample_autotools_app"
    runtime_env = os.environ.copy()

    if not system_install:
        hestia_dir = e2e_dir / "hestia"
        pkgconfig_path = f"{hestia_dir}/lib/pkgconfig"
        runtime_env["PKG_CONFIG_PATH"] = pkgconfig_path

    cmd = f"./autogen.sh; ./configure; make;"
    subprocess.run(cmd, shell=True, cwd=runtime_path, env=runtime_env)

def setup_environment(source_dir: Path, e2e_dir: Path):
    test_data_src = source_dir / "test" / "data"
    test_data_dest = e2e_dir / "test_data"

    logging.info(f"Copying test data from: {test_data_src} to {test_data_dest}")
    shutil.copytree(test_data_src, test_data_dest)

    cmake_sample_src = source_dir / "examples" / "sample_cmake_app"
    cmake_sample_dest = e2e_dir / "sample_cmake_app"
    logging.info(f"Copying sample app from: {cmake_sample_src} to {cmake_sample_dest}")
    shutil.copytree(cmake_sample_src, cmake_sample_dest)

    autotools_sample_src = source_dir / "examples" / "sample_autotools_app"
    autotools_sample_dest = e2e_dir / "sample_autotools_app"
    logging.info(f"Copying sample app from: {autotools_sample_src} to {autotools_sample_dest}")
    shutil.copytree(autotools_sample_src, autotools_sample_dest)

def run_tests(source_dir: Path, e2e_dir: Path, package_path: Path):

    logging.info("Installing package")
    system_install = install_package(e2e_dir, package_path)

    setup_environment(source_dir, e2e_dir)

    run_cli_tests(e2e_dir)

    run_cmake_sample_app(e2e_dir, system_install)

    run_autotools_sample_app(e2e_dir, system_install)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())
    parser.add_argument('--package_name', type=str, required=True)

    args = parser.parse_args()

    build_dir = Path(args.build_dir)
    e2e_dir = build_dir / "e2e_tests"
    package_path = build_dir / args.package_name

    if (Path.exists(e2e_dir)):
        print(f"E2E Tests: clearing {e2e_dir}")
        shutil.rmtree(e2e_dir)
    
    Path.mkdir(e2e_dir)

    logging.basicConfig(filename=e2e_dir / 'e2e_tests.log', filemode='w', 
                        format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                        level=logging.INFO)

    logging.info("Starting E2E Tests")

    run_tests(Path(args.source_dir), e2e_dir, package_path)

    logging.info("Finished E2E Tests")