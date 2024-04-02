import os
import argparse
import logging
import shutil
from pathlib import Path

import hestia_tests.utils
from hestia_tests.package import Package

from hestia_tests.cli_tests import CliTests
from hestia_tests.sample_app_tests import SampleAppTests
from hestia_tests.s3_api_tests import S3ApiTestFixture
from hestia_tests.service_tests import ServiceTests
from hestia_tests.rest_api_tests import RestApiTestFixture

class E2eTests():
    def __init__(self, source_dir: Path, project_dir: Path, log_to_console: bool):
        self.source_dir = source_dir
        self.project_dir = project_dir
        self.log_to_console = log_to_console
        self.project_name = "hestia"

        self.work_dir = self.project_dir / "e2e_tests"
        print("E2E Tests work dir is: " + str(self.work_dir))
        Path.mkdir(self.work_dir, parents=True, exist_ok=True)

        hestia_tests.utils.setup_default_logging(self.work_dir / 'e2e_tests.log', log_to_console)

    def setup_environment(self):
        test_data_src = self.source_dir / "test" / "data"
        test_data_dest = self.work_dir / "test_data"
        logging.info(f"Copying test data from: {test_data_src} to {test_data_dest}")

        if test_data_dest.exists():
            shutil.rmtree(test_data_dest)
        shutil.copytree(test_data_src, test_data_dest)

    def run(self):
        self.setup_environment()

        logging.info(f"Installing packages, searching in: {self.project_dir}")

        package = Package(self.project_dir, self.work_dir, self.project_name)
        archive_installed, system_installed = package.install()

        logging.info("Creating tests")
        self.archive_tests = []
        self.system_tests = []

        if archive_installed:
            installation_dir = self.work_dir / self.project_name
            self.archive_tests = [
                            CliTests(installation_dir, self.work_dir, False), 
                            SampleAppTests(self.source_dir, installation_dir, self.work_dir, False),
                            
                            S3ApiTestFixture(installation_dir, self.work_dir, False),
                            RestApiTestFixture(installation_dir, self.work_dir, False),                        
                            # ServiceTests(installation_dir, self.work_dir, False),
                        ]

        logging.info(f"Running {len(self.archive_tests)} archive tests")
        for eachTest in self.archive_tests:
            eachTest.run()

        if system_installed:
            self.system_tests = [
                            CliTests(self.project_dir, self.work_dir, True), 
                            SampleAppTests(self.source_dir, self.project_dir, self.work_dir, True),
                            
                            S3ApiTestFixture(self.project_dir, self.work_dir, True),
                            RestApiTestFixture(self.project_dir, self.work_dir, True),                            
                            #ServiceTests(self.project_dir, self.work_dir, system_install),
                        ]

        logging.info(f"Running {len(self.system_tests)} system tests")
        for eachTest in self.system_tests:
            eachTest.run()

        logging.info("Finished E2E Tests")

def get_absolute_path(path: str):
    dir = Path(path)
    if not dir.is_absolute():
        dir = os.getcwd() / dir
    return dir

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())
    parser.add_argument('--log_to_console', type=bool, default=False)

    args = parser.parse_args()

    source_dir = get_absolute_path(args.source_dir)
    build_dir = get_absolute_path(args.build_dir)
    log_to_console = args.log_to_console

    e2e_tests = E2eTests(source_dir, build_dir, log_to_console)
    e2e_tests.run()