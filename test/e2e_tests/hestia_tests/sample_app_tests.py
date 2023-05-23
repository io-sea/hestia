import os
from pathlib import Path
import logging
import subprocess
import shutil

import hestia_tests.utils

class SampleAppTests(hestia_tests.utils.BaseTest):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)

        self.name = "sample_app_tests"

    def setup_environment(self):
        cmake_sample_src = self.source_dir / "examples" / "sample_cmake_app"
        cmake_sample_dest = self.work_dir / "sample_cmake_app"
        logging.info(f"Copying sample app from: {cmake_sample_src} to {cmake_sample_dest}")
        shutil.copytree(cmake_sample_src, cmake_sample_dest)

        autotools_sample_src = self.source_dir / "examples" / "sample_autotools_app"
        autotools_sample_dest = self.work_dir / "sample_autotools_app"
        logging.info(f"Copying sample app from: {autotools_sample_src} to {autotools_sample_dest}")
        shutil.copytree(autotools_sample_src, autotools_sample_dest)

    def run(self):
        self.run_cmake_sample_app()
        self.run_autotools_sample_app()

    def run_cmake_sample_app(self):
        logging.info("Running CMake sample app")

        runtime_path = self.work_dir / "sample_cmake_app"
        build_dir = runtime_path / "build"

        if not self.system_install:
            project_dir = self.work_dir / self.project_name
            cmake_args = f"-D{self.project_name}_DIR={project_dir}/lib/cmake/{self.project_name} "
        else:
            cmake_args = ""

        cmd = f"mkdir {build_dir}; cd {build_dir}; cmake {cmake_args}../; make; ./{self.project_name}_sample_app"
        subprocess.run(cmd, shell=True, cwd=runtime_path)

    def run_autotools_sample_app(self):

        logging.info("Running Autotools sample app")

        runtime_path = self.work_dir / "sample_autotools_app"
        runtime_env = os.environ.copy()

        if not self.system_install:
            project_dir = self.work_dir / self.project_name
            pkgconfig_path = f"{project_dir}/lib/pkgconfig"
            runtime_env["PKG_CONFIG_PATH"] = pkgconfig_path

        cmd = f"./autogen.sh; ./configure; make;"
        subprocess.run(cmd, shell=True, cwd=runtime_path, env=runtime_env)