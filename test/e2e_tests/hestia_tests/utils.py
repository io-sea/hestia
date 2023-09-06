from pathlib import Path
import logging
import tarfile
import subprocess
import sys
import requests
import os

class BaseTest(object):

    def __init__(self, source_dir: Path, work_dir: Path, system_install: bool = False, project_name: str = "hestia"):
        self.source_dir = source_dir
        self.work_dir = work_dir
        self.system_install = system_install
        self.project_name = project_name
        self.name = None

    def insert_project_paths(self, environ):
        project_bin_dir = self.work_dir / self.project_name / "bin"
        project_lib_dir = self.work_dir / self.project_name / "lib"
        environ["PATH"] = f"{project_bin_dir}:{environ['PATH']}"

        if "LD_LIBRARY_PATH" in environ:
            environ["LD_LIBRARY_PATH"] = f"{project_lib_dir}:{environ['LD_LIBRARY_PATH']}"
        else:
            environ["LD_LIBRARY_PATH"] = f"{project_lib_dir}"
        return environ

    def setup_environment(self):
        pass

    def run(self):
        pass

class HttpClient(object):

    def __init__(self, endpoint):
        self.endpoint = endpoint

    def get(self, url):
        r = requests.get(self.endpoint + url)
        self.print_response(r)

    def put(self, url, data):
        r = requests.put(self.endpoint + url, data)
        self.print_response(r)
    
    def print_response(self, r):
        print(r.status_code)
        print(r.headers)
        print(r.text)  

class BasicS3Client(object):

    def __init__(self, endpoint):
        self.endpoint = endpoint

    def list_buckets(self):
        r = requests.get(self.endpoint)
        self.print_response(r)

    def get_bucket(self, bucket_name):
        r = requests.get(self.endpoint + bucket_name)
        self.print_response(r)

    def get_object(self, bucket_name, object_name):
        r = requests.get(self.endpoint + bucket_name + "/" + object_name)
        self.print_response(r)

    def put_object(self, bucket_name, object_name, data=None):
        if data:
            r = requests.put(self.endpoint + bucket_name + "/" + object_name, data=data)
        else:
            r = requests.put(self.endpoint + bucket_name + "/" + object_name)
        self.print_response(r)

    def put_bucket(self, bucket_name):
        r = requests.put(self.endpoint + bucket_name)
        self.print_response(r)

    def delete_bucket(self, bucket_name):
        r = requests.delete(self.endpoint + bucket_name)
        self.print_response(r)        

    def print_response(self, r):
        print(r.status_code)
        print(r.headers)
        print(r.text)     


def find_package(work_dir: Path) -> Path:
    if sys.platform == "darwin":
        search_extensions = [".tar.gz"]
    else:
        search_extensions = [".rpm", ".deb", ".tar.gz"]
    package_paths = []
    for entry in work_dir.iterdir():
        if entry.is_file():
            for ext in search_extensions:
                if str(entry).endswith(ext):
                    package_paths.append(entry)
    return package_paths

def install_package(work_dir: Path, package_path: Path, devel_package_path: Path, project_name: str) -> bool:
    system_install = False
    if str(package_path).endswith(".tar.gz"):
        logging.info(f"Extracting: {package_path} to {work_dir}")
        tar = tarfile.open(package_path)
        tar.extractall(work_dir)
        tar.close()

        package_name = package_path.stem

        # Remove .tar from package name
        components = package_path.stem.split(".")
        package_name = package_name[0:-4]

        extracted_path = work_dir / package_name
        extracted_path.rename(work_dir / project_name)

    elif str(package_path).endswith(".rpm"):
        logging.info(f"Installing RPMs: {package_path}, {devel_package_path}")

        cmd = f"yum install -y {package_path} {devel_package_path}"
        subprocess.run(cmd, shell=True)
        logging.info(f"RPM installed")
        system_install = True
    return system_install

def run_ops(runtime_path: Path, runtime_env, ops: list):
    result_output = []
    for op in ops:
        logging.info("Launching: " + op)
        results = subprocess.run(op, shell=True, cwd=runtime_path, env=runtime_env, stdout=subprocess.PIPE)
        if results.returncode != 0:
            logging.info("Failed running: " + op)
            raise RuntimeError("External process call failed with code: " + str(results.returncode))
        result_output.append(results.stdout)
        logging.info("Finished CLI command")
    return result_output