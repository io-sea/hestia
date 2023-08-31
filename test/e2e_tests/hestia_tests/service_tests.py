import os
from pathlib import Path
import logging
import shutil
import filecmp
import json

import argparse

import hestia_tests.utils

class ServiceTests(hestia_tests.utils.BaseTest):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)

        self.name = "service_tests"

        self.runtime_path = self.work_dir + "/service_tests"
        self.runtime_env = os.environ.copy()

    def run(self):
        logging.info("Running Service tests")

        self.runtime_path = self.work_dir / "service_tests"
        Path.mkdir(self.runtime_path)
        
        if not self.system_install:
            self.runtime_env = self.insert_project_paths(self.runtime_env)

        start_service = "systemctl start hestiad"
        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [start_service])

        check_service = "systemctl is-active hestiad"
        is_active = hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [check_service])
        if is_active[0] != "active":
            raise RuntimeError(f'Hestia Service failed to start')

        start_service = "systemctl restart hestiad"
        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [start_service])

        check_service = "systemctl is-active hestiad"
        is_active = hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [check_service])
        if is_active[0] != "active":
            raise RuntimeError(f'Hestia Service failed to restart')

        stop_service = "systemctl stop hestiad"

        check_service = "systemctl is-active hestiad"
        is_active = hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [check_service])
        if is_active[0] != "inactive":
            raise RuntimeError(f'Hestia Service failed to stop')

        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [stop_service])

        
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())

    args = parser.parse_args()

    tests = ServiceTests(args.source_dir, args.build_dir, False)
    tests.run_commands()