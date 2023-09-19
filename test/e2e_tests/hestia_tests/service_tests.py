import os
import sys
from pathlib import Path
import logging
import shutil
import filecmp
import json

import argparse

from hestia_tests.fixtures.base_test_fixture import BaseTestFixture

class ServiceTests(BaseTestFixture):

    def __init__(self, project_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(project_dir, work_dir, system_install)

        self.name = "service_tests"
        self.service_name = "hestiad"

        if self.supported():
            self.setup() 

    def is_root(self):
        return os.geteuid() == 0
    
    def supported(self):
        return sys.platform != "darwin" and self.is_root()

    def check_if_active(self):
        check_service = f"systemctl is-active {self.service_name}"
        is_active = self.run_ops([check_service])
        return is_active[0] == "active"

    def run(self):
        if not self.supported():
            logging.info("Skipping Service tests - not supported in this environment")
            return

        logging.info("Running Service tests")

        start_service = f"systemctl start {self.service_name}"
        self.run_ops([start_service])
        if not self.check_if_active():
            raise RuntimeError(f'Hestia Service failed to start')

        restart_service = f"systemctl restart {self.service_name}"
        self.run_ops([restart_service])
        if not self.check_if_active():
            raise RuntimeError(f'Hestia Service failed to restart')

        stop_service = f"systemctl stop {self.service_name}"
        self.run_ops([stop_service])

        if self.check_if_active():
            raise RuntimeError(f'Hestia Service failed to stop')
        
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--project_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())

    args = parser.parse_args()

    tests = ServiceTests(args.project_dir, args.build_dir, False)
    tests.run_commands()