import subprocess
import time

def is_redis_running():
    op = "redis-cli ping"
    results = subprocess.run(op, shell=True, stdout=subprocess.PIPE) 
    if results.returncode != 0:
        return False
    print("ping check: " + results.stdout.decode("utf-8") )
    return "PONG" in results.stdout.decode("utf-8")

def start_redis():
    if is_redis_running():
        return
    
    op = "redis-server"
    subprocess.Popen(op, shell=True, stdout=None, stderr=None, stdin=None, close_fds=True)  
    print("Started redis")
    time.sleep(0.2)
    while not is_redis_running():
        time.sleep(0.2)

def stop_redis():
    op = "redis-cli shutdown"
    results = subprocess.run(op, shell=True, stdout=subprocess.PIPE) 
    if results.returncode != 0:
        raise RuntimeError("Failed to start redis with: " + str(results.returncode))   