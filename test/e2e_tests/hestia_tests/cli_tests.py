import os
from pathlib import Path
import logging
import filecmp
import json

import hestia_tests.utils
import hestia_tests.yaml_utils
from hestia_tests.fixtures.base_test_fixture import BaseTestFixture

class CliTests(BaseTestFixture):

    def __init__(self, project_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(project_dir, work_dir, system_install)
        self.name = "cli_tests"
        
        self.setup()

    def write_temp_attrs(self, path, attrs):
        with open(path, 'w') as f:
            for key, value in attrs.items():
                f.write("data." + key + "=" + value + "\n")

    def check_event_feed(self, id, tag):
        path = self.runtime_path / "cache" / "event_feed.yaml"
        last_event = hestia_tests.yaml_utils.load_last_event(path)
        if last_event.tag != tag:
            raise ValueError("Unexpected last event type in event feed: " + last_event.tag)
        if last_event.id != id:
            raise ValueError("Unexpected last event id in event feed: " + last_event.id)
        
    def process_shell_result(self, result):
        return result[0].decode("utf-8").rstrip()
        
    def create_object(self, id: str = None):
        create_cmd = f"hestia object create"
        if id is not None:
            create_cmd += " " + id
        created_id = self.process_shell_result(self.run_ops([create_cmd]))
        self.check_event_feed(created_id, "create")
        return created_id
    
    def add_metadata(self, id: str, attrs: dict):
        temp_attr_path = self.runtime_path / "temp_attrs.dat"
        self.write_temp_attrs(temp_attr_path, attrs)

        update_cmd = f"hestia metadata update --input_fmt=key_value --id_fmt=parent_id {id} < {temp_attr_path}"
        results = self.run_ops([update_cmd])
        os.remove(temp_attr_path)

        self.check_event_feed(id, "update")
        return self.process_shell_result(results)
    
    def read_action(self, id: str):
        read_cmd = f"hestia action read --output_fmt=json {id}"
        results = self.run_ops([read_cmd])
        return json.loads(self.process_shell_result(results))

    def put_data(self, id: str, path: Path):
        put_cmd = f"hestia object put_data {id} --file={path}"
        results = self.run_ops([put_cmd])
        action_id = self.process_shell_result(results)

        action_json = self.read_action(action_id)
        if action_json["status"] != "finished_ok":
            raise ValueError("Put action did not complete ok")
        return action_id
    
    def get_data(self, id: str, path: Path, tier: int = 0):
        get_cmd = f"hestia object get_data {id} --file={path} --tier={tier}"
        results = self.run_ops([get_cmd])
        action_id = self.process_shell_result(results)

        action_json = self.read_action(action_id)
        if action_json["status"] != "finished_ok":
            raise ValueError("Get action did not complete ok")
        return action_id
    
    def copy_data(self, id: str, source_tier: int, target_tier: int):
        copy_cmd = f"hestia object copy_data {id} --source={source_tier} --target={target_tier}"
        results = self.run_ops([copy_cmd])
        action_id = self.process_shell_result(results)

        action_json = self.read_action(action_id)
        if action_json["status"] != "finished_ok":
            raise ValueError("Copy action did not complete ok")
        return action_id
    
    def move_data(self, id: str, source_tier: int, target_tier: int):
        copy_cmd = f"hestia object move_data {id} --source={source_tier} --target={target_tier}"
        results = self.run_ops([copy_cmd])
        action_id = self.process_shell_result(results)

        action_json = self.read_action(action_id)
        if action_json["status"] != "finished_ok":
            raise ValueError("Move action did not complete ok")
        return action_id
    
    def release_data(self, id: str, tier: int):
        release_cmd = f"hestia object release_data {id} --tier={tier}"
        results = self.run_ops([release_cmd])
        action_id = self.process_shell_result(results)

        action_json = self.read_action(action_id)
        if action_json["status"] != "finished_ok":
            raise ValueError("Release action did not complete ok")
        return action_id

    def run(self):
        object_content = self.get_test_data("EmperorWu.txt")
        returned_object = self.runtime_path / "returned_object.dat"

        object_id = self.create_object()
        logging.info("Created object with id: " + object_id)

        attrs = {"mykey0" : "myval0",
                 "mykey1" : "myval1"}
        put_attr_response = self.add_metadata(object_id, attrs)
        logging.info("Updated object metadata")

        put_action = self.put_data(object_id, object_content)
        copy_action = self.copy_data(object_id, 0, 1)
        move_action = self.move_data(object_id, 1, 2)
        release_action = self.release_data(object_id, 0)
        get_action = self.get_data(object_id, returned_object, 2)

        self.compare_files(object_content, returned_object, 'Object content mismatch after put-get')
        return
        
if __name__ == "__main__":

    work_dir = Path(os.getcwd())
    project_dir = work_dir.parent

    hestia_tests.utils.setup_default_logging(work_dir / 'cli_tests.log')

    test_fixture = CliTests(project_dir, work_dir, False)
    test_fixture.run()