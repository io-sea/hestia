from pathlib import Path
import logging
import tarfile
import subprocess
import sys

class BaseTest(object):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool = False, project_name: str = "hestia"):
        self.source_dir = source_dir
        self.work_dir = work_dir
        self.system_install = system_install
        self.project_name = project_name
        self.name = None

    def insert_project_paths(self, environ):
        project_bin_dir = self.work_dir / self.project_name / "bin"
        project_lib_dir = self.work_dir / self.project_name / "lib"
        environ["PATH"] = f"{project_bin_dir}:{environ['PATH']}"

        if "LD_LIBRARY_PATH" in environ:
            environ["LD_LIBRARY_PATH"] = f"{project_lib_dir}:{environ['LD_LIBRARY_PATH']}"
        else:
            environ["LD_LIBRARY_PATH"] = f"{project_lib_dir}"
        return environ

    def setup_environment(self):
        pass

    def run(self):
        pass


def find_package(work_dir: Path) -> Path:
    if sys.platform == "darwin":
        search_extensions = [".tar.gz"]
    else:
        search_extensions = [".rpm", ".deb", ".tar.gz"]

    for entry in work_dir.iterdir():
        if entry.is_file():
            for ext in search_extensions:
                if str(entry).endswith(ext):
                    return entry
    return None

def install_package(work_dir: Path, package_path: Path, project_name: str) -> bool:
    system_install = False
    if str(package_path).endswith(".tar.gz"):
        logging.info(f"Extracting: {package_path} to {work_dir}")
        tar = tarfile.open(package_path)
        tar.extractall(work_dir)
        tar.close()

        package_name = package_path.stem

        # Remove .tar from package name
        components = package_path.stem.split(".")
        package_name = package_name[0:-4]

        extracted_path = work_dir / package_name
        extracted_path.rename(work_dir / project_name)

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