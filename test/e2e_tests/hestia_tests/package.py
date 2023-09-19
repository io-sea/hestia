import sys
import logging 
import tarfile
import shutil
from pathlib import Path
import subprocess

class Package(object):

    def __init__(self, project_dir, work_dir, project_name) -> None:
        self.project_dir = project_dir
        self.work_dir = work_dir
        self.project_name = project_name

    def install(self) -> bool:
        packages = self.find_packages()

        rpm_packages = [p for p in packages if p.suffix == ".rpm"]
        archives = [p for p in packages if str(p).endswith(".tar.gz")]

        installed_archive = False
        installed_system = False
        archive_suffix = "Linux"
        if sys.platform == "darwin":
            archive_suffix = "Darwin"

        for archive in archives:
            if archive_suffix in str(archive):
                self.install_tgz(archive)
                installed_archive = True
                break

        for rpm in rpm_packages:
            if ".src.rpm" not in str(rpm):
                self.install_rpm(rpm)
                installed_system = True
        return installed_archive, installed_system
        
    def find_packages(self) -> Path:
        if sys.platform == "darwin":
            search_extensions = [".tar.gz"]
        else:
            search_extensions = [".rpm", ".deb", ".tar.gz"]
        package_paths = []
        for entry in self.project_dir.iterdir():
            if entry.is_file():
                for ext in search_extensions:
                    if str(entry).endswith(ext):
                        package_paths.append(entry)
        return package_paths
    
    def install_rpm(self, package_path: Path):
        devel_package_path = package_path
        logging.info(f"Installing RPMs: {package_path}, {devel_package_path}")

        cmd = f"dnf install -y {package_path} {devel_package_path}"
        subprocess.run(cmd, shell=True)
        logging.info(f"RPM installed")
    
    def install_tgz(self, package_path: Path):
        installed_path = self.work_dir / self.project_name
        if installed_path.is_dir():
            shutil.rmtree(installed_path)

        logging.info(f"Extracting: {package_path} to {self.work_dir}")
        tar = tarfile.open(package_path)
        tar.extractall(self.work_dir)
        tar.close()

        package_name = package_path.stem

        # Remove .tar from package name
        components = package_path.stem.split(".")
        package_name = package_name[0:-4]
        extracted_path = self.work_dir / package_name
        extracted_path.rename(installed_path)