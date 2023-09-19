import os
from pathlib import Path
import logging
import subprocess
import shutil
import argparse

import hestia_tests.utils
from hestia_tests.fixtures.base_test_fixture import BaseTestFixture

class SampleAppTests(BaseTestFixture):

    def __init__(self, source_dir: Path, project_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(project_dir, work_dir, system_install)

        self.source_dir = source_dir
        self.name = "sample_app_tests"

        self.setup()
        self.copy_examples()

    def copy_examples(self):
        cmake_sample_src = self.source_dir / "examples/sample_cmake_app"
        cmake_sample_dest = self.runtime_path / "sample_cmake_app"
        logging.info(f"Copying sample app from: {cmake_sample_src} to {cmake_sample_dest}")
        shutil.copytree(cmake_sample_src, cmake_sample_dest)

        autotools_sample_src = self.source_dir / "examples/sample_autotools_app"
        autotools_sample_dest = self.runtime_path / "sample_autotools_app"
        logging.info(f"Copying sample app from: {autotools_sample_src} to {autotools_sample_dest}")
        shutil.copytree(autotools_sample_src, autotools_sample_dest)

    def run_cmake_sample_app(self):
        logging.info("Running CMake sample app")

        runtime_path = self.runtime_path / "sample_cmake_app"
        build_dir = runtime_path / "build"

        cmake_args = ""
        if not self.system_install:
            project_dir = self.project_dir / self.project_name
            cmake_args = f"-D{self.project_name}_DIR={project_dir}/lib/cmake/{self.project_name} "

        sample_apps = [f"{self.project_name}_sample_app_cpp",
                       f"{self.project_name}_sample_app_c"]
        sample_app_str = ""
        for app in sample_apps:
            sample_app_str += f"./{app}; "

        commands = [[f"mkdir {build_dir}", runtime_path],
                    [f"cmake {cmake_args} {runtime_path}", build_dir],
                    ["make", build_dir],
                    [sample_app_str, build_dir]]
        for command, path in commands:
            logging.info("Running: " + command + " in " + str(path))
            result = subprocess.run(command, shell=True, cwd=path, env=self.runtime_env)
            if result.returncode != 0:
                logging.error("Failed running: " + command)
                raise RuntimeError("External process call failed with code: " + str(result.returncode))

    def run_autotools_sample_app(self):
        logging.info("Running Autotools sample app")

        runtime_path = self.runtime_path / "sample_autotools_app"

        commands = ["./autogen.sh",
                    "./configure",
                    "make",
                    f"./{self.project_name}_sample_app"]
        for command in commands:
            logging.info("Running: " + command)
            result = subprocess.run(command, shell=True, cwd=runtime_path, env=self.runtime_env)
            if result.returncode != 0:
                logging.error("Failed running: " + command)
                raise RuntimeError("External process call failed with code: " + str(result.returncode))

    def run(self):
        self.run_cmake_sample_app()
        self.run_autotools_sample_app()

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str)
    parser.add_argument('--install_dir', type=str)
    args = parser.parse_args()

    work_dir = Path(os.getcwd())
    project_dir = Path(args.install_dir)
    source_dir = Path(args.source_dir)

    hestia_tests.utils.setup_default_logging(work_dir / 'sample_app_tests.log')

    test_fixture = SampleAppTests(source_dir, project_dir, work_dir, False)
    test_fixture.run()