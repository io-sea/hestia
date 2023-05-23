import os
import sys
import argparse
import logging
import shutil
from pathlib import Path
import subprocess

from hestia_tests.utils import BaseTest, install_package, find_package

from hestia_tests.cli_tests import CliTests
from hestia_tests.sample_app_tests import SampleAppTests
from hestia_tests.server_tests import ServerTests
# from hestia_tests.s3_tests import S3Tests

class E2eTests(BaseTest):
    def __init__(self, source_dir: Path, work_dir: Path, package_path: Path):
        super().__init__(source_dir, work_dir)
        self.package_path = package_path

    def setup_environment(self):
        test_data_src = self.source_dir / "test" / "data"
        test_data_dest = self.work_dir / "test_data"

        logging.info(f"Copying test data from: {test_data_src} to {test_data_dest}")
        shutil.copytree(test_data_src, test_data_dest)

    def run(self, test_names = []):
        logging.info("Installing package")
        self.system_install = install_package(self.work_dir, self.package_path, self.project_name)

        self.tests = [
                        CliTests(self.source_dir, self.work_dir, self.system_install), 
                        SampleAppTests(self.source_dir, self.work_dir, self.system_install),
                        # ServerTests(self.source_dir, self.work_dir, self.system_install),
                        #S3Tests(self.source_dir, self.work_dir, self.system_install)
                    ]
        
        for eachTest in self.tests:
            if (not test_names) or (eachTest.name in test_names):
                eachTest.setup_environment()
                eachTest.run()

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())
    parser.add_argument('--package_name', type=str)

    args = parser.parse_args()

    source_dir = Path(args.source_dir)
    if not source_dir.is_absolute():
        source_dir = os.getcwd() / source_dir

    build_dir = Path(args.build_dir)
    if not build_dir.is_absolute():
        build_dir = os.getcwd() / build_dir

    work_dir = build_dir / "e2e_tests"
    if (Path.exists(work_dir)):
        print(f"E2E Tests: clearing {work_dir}")
        shutil.rmtree(work_dir)
    
    Path.mkdir(work_dir)

    logging.basicConfig(filename=work_dir / 'e2e_tests.log', filemode='w', 
                        format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                        level=logging.INFO)
    
    logging.info("Starting E2E Tests")
    
    if args.package_name is not None:
        package_path = build_dir / args.package_name
    else:
        logging.info(f"Package name not given, searching in: {build_dir}")
        package_path = find_package(build_dir)
        if package_path is None:
            raise ValueError("Failed to find supported package")
        else:
            logging.info(f"Found package: {package_path}")

    e2e_tests = E2eTests(source_dir, work_dir, package_path)
    e2e_tests.setup_environment()
    e2e_tests.run()

    logging.info("Finished E2E Tests")