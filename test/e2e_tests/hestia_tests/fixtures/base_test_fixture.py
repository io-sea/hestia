import sys
import os
import psutil
from pathlib import Path
import shutil
import logging
import subprocess
import filecmp

class BaseTestFixture(object):

    def __init__(self, project_dir: Path, work_dir: Path, 
                 system_install: bool = False, 
                 project_name: str = "hestia"):
        self.project_dir = project_dir
        self.work_dir = work_dir
        self.system_install = system_install
        self.project_name = project_name
        self.name = None

        self.runtime_path = None
        self.runtime_env = None 

    def setup(self):
        logging.info("Setting up tests: " + self.name)

        self.runtime_path = self.work_dir / self.name
        if self.runtime_path.is_dir():
            shutil.rmtree(self.runtime_path)
        Path.mkdir(self.runtime_path, parents=True)

        if self.runtime_env is None:
            self.runtime_env = os.environ.copy()

        self.insert_project_paths()
        self.runtime_env["HESTIA_CACHE_DIR"] = self.runtime_path / "cache"
    
    def insert_project_paths(self):
        libdir = "lib"
        if sys.platform != "darwin":
            libdir = "lib64"

        if not self.system_install:
            project_bin_dir = self.project_dir / "bin"
            project_lib_dir = self.project_dir / libdir
            self.runtime_env["PATH"] = f"{project_bin_dir}:{self.runtime_env['PATH']}"
            self.set_ld_library_path(project_lib_dir)
            self.set_pkg_conf_path(project_lib_dir / "pkgconfig")
        else:
            self.set_ld_library_path(f"/usr/{libdir}/hestia/")
            self.set_pkg_conf_path(f"/usr/{libdir}/pkgconfig/")

    def set_ld_library_path(self, path):
        env_name = "LD_LIBRARY_PATH"
        if sys.platform == "darwin":
            env_name = "DYLD_LIBRARY_PATH"
        if env_name in self.runtime_env:
            self.runtime_env[env_name] = str(path) + ":" + self.runtime_env[env_name]
        else:
            self.runtime_env[env_name] = str(path)

    def set_pkg_conf_path(self, path):
        self.runtime_env["PKG_CONFIG_PATH"] = path
    
    def get_test_data(self, filename):
        object_content = self.work_dir / "test_data" / filename
        if not object_content.is_file():
            object_content = self.project_dir / "test_data" / filename
        return object_content
    
    def get_process_on_port(self, name, port):
        processes = [proc for proc in psutil.process_iter() if proc.name()
                    == name]
        for p in processes:
            for c in p.connections():
                if c.status == 'LISTEN' and c.laddr.port == port:
                    return p
        return None
    
    def string_from_shell_result(self, result):
        flat_result = ""
        for line in result:
            flat_result += line.decode("utf-8").rstrip()
        return flat_result

    def run_ops(self, ops: list, wait: bool = True):
        result_output = []
        for op in ops:
            logging.info("Launching: " + op)
            if wait:
                results = subprocess.run(op, shell=True, 
                                         cwd=self.runtime_path, 
                                         env=self.runtime_env, 
                                         stdout=subprocess.PIPE)
                if results.returncode != 0:
                    logging.info("Failed running: " + op)
                    raise RuntimeError(f"External process call failed with code: {results.returncode}")
                result_output.append(results.stdout)
            else:
                subprocess.run(op, shell=True, cwd=self.runtime_path, env=self.runtime_env)
            logging.info("Finished CLI command")
        return result_output
    
    def start_process(self, op):
        logging.info("Starting process: " + op)
        return subprocess.Popen(op, shell=True, 
                                cwd=self.runtime_path, 
                                env=self.runtime_env)

    def compare_files(self, path0, path1, msg):
        if not filecmp.cmp(path0, path1):
            raise ValueError(msg)
        return