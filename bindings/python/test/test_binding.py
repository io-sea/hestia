import hestia
import hestia.hestia_server
import hestia.hestia_client
import os
import time
import signal
import sys
import shutil

class BaseTestFixture():
    def __init__(self) -> None:
        self.cache_path = os.getcwd() + "/.cache"
        self.clean_cache()
        
        self.client = None

    def clean_cache(self):
        if os.path.isdir(self.cache_path):
            shutil.rmtree(self.cache_path)

    def do_in_memory_object_ops(self):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read_ids([object_id])

        content = b"The quick brown fox jumps over the lazy dog."
        action_id = self.client.object_put(object_id, content)

        return

        action = self.client.action_read_ids([action_id])
        print("Put action: " + str(action))

        return

        retrieved = self.client.object_get(object_id, len(content))
        print(retrieved)

        copied = self.client.object_copy(object_id, 0, 1)
        print(copied)

        moved = self.client.object_move(object_id, 1, 2)
        print(moved)

        released = self.client.object_release(object_id, 0)
        print(released)

        retrieved = self.client.object_get(object_id, len(content), 0, 2)
        print(retrieved)

    def do_fd_object_ops(self, path):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read()

        file_size = os.path.getsize(path)

        with open(path, 'r') as f:
            action_id = self.client.object_put_fd(object_id, f.fileno(), file_size)

        with open(path + ".copied", 'w') as f:
            action_id = self.client.object_get_fd(object_id, f.fileno(), file_size)

        print("Action id: " + action_id)

    def do_path_object_ops(self, path):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read()

        file_size = os.path.getsize(path)

        action_id = self.client.object_put_path(object_id, path)
        print("Put action:" + action_id)

        output_path = path + ".copied"
        action_id = self.client.object_get_path(object_id, output_path)
        print("Get Action id: " + action_id)


class ControllerWithStorageFixture(BaseTestFixture):

    def __init__(self) -> None:
        super().__init__()
        self.server = hestia.hestia_server.HestiaServerWrapper()
        self.server.start_controller_with_storage()
        self.client = hestia.HestiaClient(None, None, hestia.hestia_client._CLIENT_FULL_CONFIG)

    def __del__(self):
        self.stop()

    def stop(self):
        self.server.stop()

class ControllerWithWorkerFixture(BaseTestFixture):

    def __init__(self) -> None:
        super().__init__()
        self.server = hestia.hestia_server.HestiaServerWrapper()
        self.server.start_controller()
        self.server.start_worker(8080)

        self.client = hestia.HestiaClient(None, None, hestia.hestia_client._CLIENT_FULL_CONFIG)

    def __del__(self):
        self.stop()

    def stop(self):
        self.server.stop()

class StandaloneClientFixture(BaseTestFixture):

    def __init__(self) -> None:
        super().__init__()
        self.client = hestia.HestiaClient()

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)

def do_standalone_fixture_tests():
    fixture = StandaloneClientFixture()
    fixture.do_in_memory_object_ops()
    #fixture.do_path_object_ops(path)
    #fixture.do_fd_object_ops(path)

def do_controller_with_storage_tests():
    fixture = ControllerWithStorageFixture()
    fixture.do_in_memory_object_ops()
    fixture.stop()

def do_controller_with_worker_tests():
    fixture = ControllerWithWorkerFixture()
    fixture.do_in_memory_object_ops()
    fixture.stop()

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    path = os.getcwd() + "/compile_commands.json"

    do_controller_with_worker_tests()















