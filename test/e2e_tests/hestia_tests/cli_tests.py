import os
from pathlib import Path
import logging
import shutil
import filecmp

import hestia_tests.utils

class CliTests(hestia_tests.utils.BaseTest):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)
        self.name = "cli_tests"

    def run(self):
        logging.info("Running CLI tests")

        object_content = self.work_dir / "test_data" / "EmperorWu.txt"
        return_cache0 = self.work_dir / "object0.dat"
        return_cache1 = self.work_dir / "object1.dat"

        runtime_path = self.work_dir / "cli_tests"
        Path.mkdir(runtime_path)

        runtime_env = os.environ.copy()
        if not self.system_install:
            runtime_env = self.insert_project_paths(runtime_env)
        
        object_id = "00001234"
        put_cmd = f"hestia put {object_id} {object_content} 0"
        get_cmd = f"hestia get {object_id} {return_cache0} 0"
        copy_cmd = f"hestia copy {object_id} 0 1"
        get1_cmd = f"hestia get {object_id} {return_cache1} 1"

        ops = [put_cmd, get_cmd, copy_cmd, get1_cmd]
        hestia_tests.utils.run_ops(runtime_path, runtime_env, ops)

        same0 = filecmp.cmp(object_content, return_cache0)
        same1 = filecmp.cmp(object_content, return_cache1)
        if not (same0 and same1):
            raise ValueError(f'Object content mismatch after put-get')