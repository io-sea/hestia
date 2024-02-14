from pathlib import Path
import os
import shutil
import argparse
import time
import logging
import subprocess

import hestia_interface as hestia

def make_file(path: Path, size):

    block_size = 64 # MB
    num_blocks = int(size / block_size) + 1

    cmd = f"dd if=/dev/urandom of={path} bs={block_size}M count={num_blocks} iflag=fullblock"
    print("Running: " + cmd)
    results = subprocess.run(cmd, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to create file with: " + str(results.returncode))   
    
def copy_file_cp(source: Path, target: Path):
    cmd = f"cp {source} {target}"
    print("Running: " + cmd)
    start = time.time()
    results = subprocess.run(cmd, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to copy file with: " + str(results.returncode))   
    end = time.time()
    return end - start

def put_file_hestia(build_dir:Path, config: Path, source: Path):
    start = time.time()
    rc = hestia.put(build_dir, source, config)
    end = time.time()
    return end - start

def get_file_hestia(build_dir:Path, config: Path, target: Path):
    start = time.time()
    rc = hestia.get(build_dir, target, config)
    end = time.time()
    return end - start

def make_files(work_dir: Path, prefix: str, sizes: list):
    for size in sizes:
        path = work_dir / Path(prefix + "_" + str(size) + ".dat")
        logging.info(f"Generating file {path}")
        make_file(path, size)

def generate_data(workdir: Path, prefix: str, sizes):
    make_files(workdir, prefix, sizes)

def copy_files_cp(workdir: Path, prefix: str, sizes):

    target_dir = workdir / "target"
    os.makedirs(target_dir, exist_ok=True)

    print("Size, Runtime")
    for size in sizes:
        filename = Path(prefix + "_" + str(size) + ".dat")
        source_path = workdir / filename
        tgt_path = target_dir / filename
        runtime = copy_file_cp(source_path, tgt_path)
        print(f"{size}, {runtime}")
        time.sleep(20)

def put_files_hestia(build_dir:Path, workdir:Path, config: Path,  prefix: str, sizes):

    print("Size, Runtime")

    for size in sizes:
        filename = Path(prefix + "_" + str(size) + ".dat")
        source_path = workdir / filename
        runtime = put_file_hestia(build_dir, config, source_path)
        print(f"{size}, {runtime}")
        time.sleep(20)

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--build_dir', type=str, default=os.getcwd())
    parser.add_argument('--config', type=str, default="hestia_config.yaml")

    args = parser.parse_args()

    sizes = [500, 1000, 2000, 4000, 8000, 16000]
    prefix = "random_content"

    build_dir = Path(args.build_dir)
    work_dir = build_dir / "large_files"
    # generate_data(work_dir), prefix, sizes)

    copy_files_cp(work_dir, prefix, sizes)

    #hestia.create(build_dir, Path(args.config))

    #put_files_hestia(build_dir, work_dir, Path(args.config), prefix, sizes)

