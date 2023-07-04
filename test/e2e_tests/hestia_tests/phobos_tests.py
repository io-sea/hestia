import os
import subprocess
import argparse
import shutil
from pathlib import Path
import logging
import filecmp
import time


import hestia_tests.utils

class PhobosTests(hestia_tests.utils.BaseTest):
    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(source_dir, work_dir, system_install)

        self.name = "phobos_tests"
    
    def setup_environment(self):
        # Assumes Phobos built at some location?
        self.phobos_install_dir = Path("/phobos")
        shutil.copytree(self.phobos_install_dir, self.work_dir / self.phobos_install_dir.name)
        self.phobos_install_dir = self.work_dir / self.phobos_install_dir.name

        # Start the phobos daemon
        cmd = f"{self.work_dir}/test_data/start_phobos_db.sh"
        logging.info("Launching:" + cmd)
        subprocess.run(cmd, shell=True, cwd=os.getcwd())

    def run(self):
        logging.info("Running Phobos tests")
        # Run some hestia commands
        object_content = self.work_dir / "test_data" / "EmperorWu.txt"
        return_cache0 = self.work_dir / "object0.dat"
        return_cache1 = self.work_dir / "object1.dat"

        runtime_path = self.work_dir / "phobos_tests"
        Path.mkdir(runtime_path)

        runtime_env = os.environ.copy()
        if not self.system_install:
            project_dir = self.work_dir / self.project_name
            runtime_env = self.insert_project_paths(runtime_env)
            runtime_env["LD_LIBRARY_PATH"] = f"{project_dir}/lib:/usr/lib64/phobos"
        else:
            runtime_env["LD_LIBRARY_PATH"] = "/usr/lib/hestia/:/usr/lib64/phobos"
        
        runtime_env["PYTHONPATH"] = f"{self.phobos_install_dir}/src/cli/build/lib.linux-x86_64-3.6"
        runtime_env["HESTIA_CACHE_DIR"] = runtime_path / "cache"
        
        hestia_config=self.work_dir / "test_data" / "configs" / "hestia_phobos_tests.yaml"

        create_cmd = f"hestia object create --config={hestia_config}"
        results = hestia_tests.utils.run_ops(runtime_path, runtime_env, [create_cmd])
        object_id = results[0].decode("utf-8").rstrip()
        logging.info("Created object with id: " + object_id)

        # cat_cmd = f"cat {runtime_path}/hestia_log.txt" # For Debug
        put_cmd = f"hestia object put {object_id} {object_content} 0 --config={hestia_config}"
        get_cmd = f"hestia object get {object_id} {return_cache0} 0 --config={hestia_config}"
        
        # TODO: Enable with copytool completion
        # copy_cmd = f"hestia copy {object_id} 0 1 --config={hestia_config}"
        # get1_cmd = f"hestia get {object_id} {return_cache1} 1 --config={hestia_config}"
        
        ops = [put_cmd, get_cmd] #, copy_cmd, get1_cmd]
        hestia_tests.utils.run_ops(runtime_path, runtime_env, ops)
        
        # Check output 
        time.sleep(5) # On CI, Phobos is slow to fully flush streams 
        filecmp.clear_cache()
        same0 = filecmp.cmp(object_content, return_cache0)
        # same1 = filecmp.cmp(object_content, return_cache1)
        if not (same0): # and same1):
            raise ValueError(f'Object content mismatch after put-get')