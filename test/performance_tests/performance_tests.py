from pathlib import Path
import os
import shutil
import argparse
import time
import logging

import mock_filesystem as mfs
import redis_interface as rs
import hestia_interface as hestia

def recreate_dir(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path, exist_ok=True)

def get_build_info(build_dir):
    build_type_prefix = "CMAKE_BUILD_TYPE:STRING="
    build_type = "Debug"
    cache_path = build_dir / "CMakeCache.txt"
    with open(cache_path, 'r') as f:
        lines = f.readlines()
        for line in lines:
            if line.startswith("CMAKE_BUILD_TYPE:STRING="):
                build_type = line[len(build_type_prefix):-1].strip()
    return build_type

def test_index_performance(source_dir, build_dir, regenerate_data):
    index_dir = build_dir / "performance_tests" / "index"

    logging.basicConfig(filename=index_dir / "test.log", filemode='w', 
                        format='%(asctime)s - %(message)s', datefmt='%d-%b-%y %H:%M:%S',
                        level=logging.INFO)

    test_run_dir = index_dir / "run"
    recreate_dir(test_run_dir)

    if regenerate_data:
        test_data_dir = index_dir / "data"
        recreate_dir(test_data_dir)
        generated_file_count = mfs.generate_random_files(test_data_dir)
        print(f'generated {generated_file_count} test files')

    build_type = get_build_info(build_dir)
    logging.info(f"Running index performance test for build type {build_type}")

    # Copy the config to runtime dir
    script_path = Path(os.path.realpath(__file__)).parent
    hestia.generate_config(script_path / "default_perf_config.yaml", test_run_dir)

    # Start Redis if needed
    logging.info(f"Starting redis")
    rs.start_redis()

    # Start the transfer
    logging.info(f"Starting transfer")
    dataset_id = hestia.archive(test_data_dir, build_dir, test_run_dir)

    # Stop Redis
    logging.info(f"Stopping redis")
    rs.stop_redis()

    # Report details

if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument('--source_dir', type=str, default=None)
    parser.add_argument('--build_dir', type=str, default=os.getcwd())
    parser.add_argument('--regenerate_data', type=bool, default=True)
    
    args = parser.parse_args()
    build_dir = Path(args.build_dir)
    source_dir = args.source_dir
    if source_dir is None:
        source_dir = build_dir / ".." / "hestia"

    test_index_performance(Path(source_dir), build_dir, args.regenerate_data)