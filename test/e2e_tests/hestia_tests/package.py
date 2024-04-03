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

    def install(self) -> tuple:
        packages = self.find_packages()

        rpm_packages = [p for p in packages if p.suffix == ".rpm"]
        archives = [p for p in packages if str(p).endswith(".tar.gz")]

        installed_archive = False
        installed_system = False
        archive_suffix = "Linux"
        if sys.platform == "darwin":
            archive_suffix = "Darwin"

        installable_archives = [a for a in archives if archive_suffix in str(a)]

        for archive in installable_archives:
            self.install_tgz(archive)
        installed_archive = bool(installable_archives)

        installable_rpms = [p for p in rpm_packages if ".src.rpm" not in str(p)]
        self.install_rpms(installable_rpms)
        installed_system = bool(installable_rpms)

        return installed_archive, installed_system

    def find_packages(self) -> list[Path]:
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

    def install_rpms(self, paths: list[Path]):
        logging.info(f"Installing {len(paths)} RPMs")
        paths_str = " ".join([str(p) for p in paths])

        cmd = f"dnf install -y {paths_str}"
        subprocess.run(cmd, shell=True, check=True)
        logging.info("RPM installed")

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
        package_name = package_name[0:-4]
        extracted_path = self.work_dir / package_name
        extracted_path.rename(installed_path)
