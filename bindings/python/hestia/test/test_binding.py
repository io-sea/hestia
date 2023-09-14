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
        object_id = "1234"

        # Create object with specified id
        object_json = self.client.object_create(object_id)
        if (object_json[0]["id"] != object_id):
            raise ValueError('Expected object id: ' + object_id + '  in Create but got ' + object_json[0]["id"])
        
        # Check that we can read it back ok
        object_read_json = self.client.object_read_ids([object_id])
        if (object_read_json["id"] != object_id):
            raise ValueError('Expected object id: ' + object_id + '  in Read but got ' + object_read_json["id"])
        
        # Update an attribute and check that it is updated ok
        object_read_json["name"] = "my_object"
        object_updated_json = self.client.object_update(object_read_json)
        if (object_updated_json["name"] != "my_object"):
            raise ValueError('Failed to update object name attr')
        
        # Remove the object
        self.client.object_remove(object_id)

        # Check that we get an empty read
        object_read_json = self.client.object_read_ids([object_id])
        if (len(object_read_json) > 0):
            raise ValueError('Expected no remaining object post delete - but found some')
        
        # Recreate the object
        object_json = self.client.object_create(object_id)

        # Set some user attributes
        object_attrs = "data.my_key0,my_value0\ndata.my_key1,my_value1"
        user_attrs = self.client.object_attrs_put(object_id, object_attrs)

        # Read the attributes back
        user_attrs_read = self.client.object_attrs_get(object_id)

        # Add some data
        content = b"The quick brown fox jumps over the lazy dog."
        action_id = self.client.object_put(object_id, content)

        # Read the action back to check the status
        action = self.client.action_read_ids([action_id])
        if (action["status"] != "finished_ok"):
            raise ValueError('Put action did not complete ok')
        
        # Get the content back
        retrieved_content = self.client.object_get(object_id, len(content))
        if (retrieved_content != content):
            raise ValueError('Retrieved content does not match original')
        
        # Copy between tiers
        copied_action_id = self.client.object_copy(object_id, 0, 1)
        copied_action = self.client.action_read_ids([copied_action_id])
        if (copied_action["status"] != "finished_ok"):
            raise ValueError('Copy action did not complete ok')

        # Move between tiers
        moved_action_id = self.client.object_move(object_id, 1, 2)
        moved_action = self.client.action_read_ids([moved_action_id])
        if (moved_action["status"] != "finished_ok"):
            raise ValueError('Copy action did not complete ok')
        
        # Release a tier
        released_action_id = self.client.object_release(object_id, 0)
        release_action = self.client.action_read_ids([released_action_id])
        if (release_action["status"] != "finished_ok"):
            raise ValueError('Copy action did not complete ok')
        
        return

    def do_fd_object_ops(self, path):
        object_json = self.client.object_create()
        object_id = object_json[0]["id"]
        print("Created object: ", object_id)

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
        self.server.start_worker()

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

    do_standalone_fixture_tests()
    #do_controller_with_storage_tests()
    #do_controller_with_worker_tests()















