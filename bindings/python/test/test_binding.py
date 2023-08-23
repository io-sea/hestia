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
        shutil.rmtree(self.cache_path)

    def do_in_memory_object_ops(self):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read()

        content = b"The quick brown fox jumps over the lazy dog."
        action_id = self.client.object_put(object_id, content)

        #action = self.client.action_read(action_id)
        print("Put action:" + action_id)

        retrieved = self.client.object_get(object_id, len(content))
        print(retrieved)

    def do_fd_object_ops(self):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read()

        file_size = os.path.getsize('Makefile')

        with open("Makefile", 'r') as f:
            activity = self.client.object_put_fd(object_id, f.fileno(), file_size)
            print(activity)

        with open("Makefile_copied", 'w') as f:
            activity = self.client.object_get_fd(object_id, f.fileno(), file_size)
            print(activity)

        action_id = self.client.object_get(object_id, file_size)
        print("Action id: " + action_id)

    def do_path_object_ops(self):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

        object_read_json = self.client.object_read()

        path = os.getcwd() + "/Makefile"
        file_size = os.path.getsize(path)

        action_id = self.client.object_put_path(object_id, path)
        #put_action = self.client.action_read(action_id)
        print("Put action:" + action_id)

        output_path = os.getcwd() + "/Makefile_copied"
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
        self.server.stop_controller()

class StandaloneClientFixture(BaseTestFixture):

    def __init__(self) -> None:
        super().__init__()
        self.client = hestia.HestiaClient()

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    sys.exit(0)


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)

    #fixture = ControllerWithStorageFixture()
    #fixture.stop()

    standalone_fixture = StandaloneClientFixture()

    standalone_fixture.do_in_memory_object_ops()
    #standalone_fixture.do_path_object_ops()













