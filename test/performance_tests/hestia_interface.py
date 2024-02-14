import subprocess

def generate_config(base_config, cache_dir):
    with open(base_config, 'r') as f:
        config = f.read()
        config += f"\n\ncache_path: {cache_dir}\n"

    with open(cache_dir / "config.yaml", 'w') as f:
        f.write(config)

def create(build_dir, config_path):
    tool_path = f"{build_dir}/bin/hestia"
    tool_args = f"object create 1234 --config {config_path}"
    op = f"{tool_path} {tool_args}"
    print("Running op " + op)
    results = subprocess.run(op, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to archive data with: " + str(results.returncode))  

def put(build_dir, source_path, config_path):
    tool_path = f"{build_dir}/bin/hestia"
    tool_args = f"object put_data 1234 --file {source_path} --config {config_path}"
    op = f"{tool_path} {tool_args}"
    print("Running op " + op)
    results = subprocess.run(op, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to archive data with: " + str(results.returncode))   
    
def get(build_dir, tgt_path, config_path):
    tool_path = f"{build_dir}/bin/hestia"
    tool_args = f"object get_data 1234 --file {tgt_path} --config {config_path}"
    op = f"{tool_path} {tool_args}"
    print("Running op " + op)
    results = subprocess.run(op, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to archive data with: " + str(results.returncode))   
    
def archive(data_dir, build_dir, cache_dir):
    tool_path = build_dir / "bin" / "hestia_sync_tool"
    config_path = cache_dir / "config.yaml"
    tool_args = f"archive --work_dir {data_dir} --config {config_path}"
    op = f"{tool_path} {tool_args}"
    print("Running op " + op)
    results = subprocess.run(op, shell=True) 
    if results.returncode != 0:
        raise RuntimeError("Failed to archive data with: " + str(results.returncode))   