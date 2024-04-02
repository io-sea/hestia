import os
from pathlib import Path
import logging
import shutil
import filecmp
import json

import argparse

import hestia_tests.utils
from hestia_tests.fixtures.server_test_fixture import ServerTestFixture

class RestApiTestFixture(ServerTestFixture):

    def __init__(self, project_dir: Path, work_dir: Path, system_install: bool):

        super().__init__(project_dir, work_dir, system_install)

        self.name = "rest_api_tests"

        self.setup()

    def create_object(self, id: str = None):
        create_cmd = self.web_client.put_cmd({"authorization" : self.token}, "objects")
        if id is not None:
            create_cmd += " " + id
        
        response = self.run_ops([create_cmd])
        print("Response is: ")
        print(response)
        object = json.loads(self.string_from_shell_result(response))[0]
        return object["id"]
    
    def add_metadata(self, id: str, attrs: dict):
        metadata = { "data" : attrs}
        body = json.dumps(metadata)

        headers = {"Content-Type" : "application/json",
                   "authorization" : self.token}

        path = f"metadatas?parent_id={id}"
    
        update_cmd = self.web_client.put_cmd(headers, path, body)
        response = self.run_ops([update_cmd])
        print ("Add metadata response is: ")
        print(self.string_from_shell_result(response))
        object = json.loads(self.string_from_shell_result(response))
        return object["id"]
    
    def read_action(self, id: str):
        headers = {"Content-Type" : "application/json",
                   "authorization" : self.token}
        
        read_cmd = self.web_client.get_cmd(headers, "actions/" + id)
        results = self.run_ops([read_cmd])
        return json.loads(self.process_shell_result(results))
    
    def put_data(self, id: str, path: Path):
        headers = {"hestia.hsm_action.action": "put_data",
                   "hestia.hsm_action.subject_key": id,
                   "authorization" : self.token}
        
        put_cmd = self.web_client.put_cmd(headers, "actions", file=str(path))

        print(put_cmd)
        response = self.run_ops([put_cmd])

        print(response)
        action_json = json.loads(self.string_from_shell_result(response))
        if action_json["status"] != "finished_ok":
            raise ValueError("Put action did not complete ok")
        return action_json["id"]

    def run(self):
        logging.info("Running Rest Api tests")

        object_content = self.get_test_data("EmperorWu.txt")
        returned_object = self.runtime_path / "returned_object.dat"

        self.start_server()

        self.login()

        object_id = self.create_object()
        logging.info("Created object with id: " + object_id)

        attrs = {"mykey0" : "myval0",
                 "mykey1" : "myval1"}
        put_attr_response = self.add_metadata(object_id, attrs)
        logging.info("Updated object metadata")

        #put_action = self.put_data(object_id, object_content)

        
if __name__ == "__main__":

    work_dir = Path(os.getcwd())
    project_dir = work_dir.parent

    hestia_tests.utils.setup_default_logging(work_dir / 'rest_api_tests.log')

    test_fixture = RestApiTestFixture(project_dir, work_dir, False)
    test_fixture.run()