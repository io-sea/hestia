import os
from pathlib import Path
import logging
import shutil
import filecmp
import boto3

import argparse

import hestia_tests.utils

class S3Tests(hestia_tests.utils.BaseTest):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)

        self.name = "s3_tests"

        self.host = "127.0.0.1:8080"
        self.api_endpoint = self.host + "/api/v1/hsm/"
        self.object_id = "000012345"

    def run(self):
        logging.info("Running S3 tests")

        object_content = self.work_dir / "test_data" / "EmperorWu.txt"
        return_cache = self.work_dir / "object.dat"

        runtime_path = self.work_dir / "s3_server_tests"
        Path.mkdir(runtime_path)

        runtime_env = os.environ.copy()
        if not self.system_install:
            runtime_env = self.insert_project_paths(runtime_env)

        start_server = "hestia start"
        hestia_tests.utils.run_ops(runtime_path, runtime_env, [start_server])

        self.run_commands()

        stop_server = "hestia stop"
        hestia_tests.utils.run_ops(runtime_path, runtime_env, [stop_server])

        same = filecmp.cmp(object_content, return_cache)
        if not same:
            raise ValueError(f'Object content mismatch after put-get')
        
    def run_commands(self):
        pass

        
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())

    args = parser.parse_args()

    tests = S3Tests(args.source_dir, args.build_dir)
    tests.setup_environment()
    tests.run()