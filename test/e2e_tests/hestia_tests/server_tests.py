import os
from pathlib import Path
import logging
import shutil
import filecmp
import json

import argparse

import hestia_tests.utils

class ServerTests(hestia_tests.utils.BaseTest):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)

        self.name = "server_tests"

        self.host = "127.0.0.1:8080"
        self.api_endpoint = self.host + "/api/v1/"
        self.object_id = "000012345"

        #object_content = self.work_dir / "test_data / EmperorWu.txt"
        #return_cache = self.work_dir / "object.dat"

        self.runtime_path = self.work_dir + "/server_tests"
        self.runtime_env = os.environ.copy()

    def run(self):
        logging.info("Running Server tests")

        object_content = self.work_dir / "test_data" / "EmperorWu.txt"
        return_cache = self.work_dir / "object.dat"

        self.runtime_path = self.work_dir / "server_tests"
        Path.mkdir(self.runtime_path)
        
        if not self.system_install:
            self.runtime_env = self.insert_project_paths(self.runtime_env)

        start_server = "hestia start"
        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [start_server])

        self.run_commands()

        # Note we use curl in the shell rather than a Python package to reflect the instructions in our user guide
        stop_server = "hestia stop"

        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, [stop_server])

        same = filecmp.cmp(object_content, return_cache)
        if not same:
            raise ValueError(f'Object content mismatch after put-get')
        
    def run_commands(self):
        #object_content = self.work_dir / "test_data/EmperorWu.txt"
        #put_cmd = f"curl -X PUT --upload-file '{object_content}' {self.api_endpoint}/objects/{self.object_id}/tiers/0"
        # get_cmd = f"curl {self.api_endpoint}/objects/{self.object_id}/tiers/0"

        node = {"id" : "1234", "app_type" : "hestia::HsmService", "host_address" : "127.0.0.1:8080"}
        node_json = json.dumps(node)

        Path.mkdir(Path(self.runtime_path), exist_ok=True)

        put_node = f"curl -H 'Content-Type: application/json' -X PUT -d '{node_json}' {self.api_endpoint}nodes"

        get_cmd = f"curl {self.api_endpoint}nodes"

        ops = [put_node, get_cmd]
        hestia_tests.utils.run_ops(self.runtime_path, self.runtime_env, ops)

        
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=os.getcwd())
    parser.add_argument('--build_dir', type=str, default=os.getcwd())

    args = parser.parse_args()

    tests = ServerTests(args.source_dir, args.build_dir, False)
    tests.run_commands()