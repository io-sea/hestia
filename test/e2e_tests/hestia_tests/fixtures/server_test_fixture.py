import os
from pathlib import Path
import logging
import json
import time

from hestia_tests.fixtures.base_test_fixture import BaseTestFixture
from hestia_tests.clients.curl_cli_client import CurlCliClient

class ServerTestFixture(BaseTestFixture):

    def __init__(self, project_dir: Path, work_dir: Path, system_install: bool):
        super().__init__(project_dir, work_dir, system_install)

        self.host = "127.0.0.1"
        self.port = 8080
        self.token = None

        self.web_client = CurlCliClient(self.host + ":" + str(self.port) + "/api/v1/")
        self.user_name = "hestia_default_user"
        self.password = "my_user_pass"
        self.server_process = None

    def __del__(self):
        try:
            self.stop_server()
        except:
            pass

    def get_config_path(self, name):
        return self.work_dir / "test_data" / f"configs/{name}.yaml"

    def start_server(self, config = None, wait_for_ping = True):
        logging.info("Starting server")

        if config is None:
            config = "hestia_controller_with_backends"

        logging.info("Using config: " + config)
        config_path = self.get_config_path(config)

        start_server_cmd = f"hestia server --config={config_path}"
        self.server_process = self.start_process(start_server_cmd)

        time.sleep(0.2)
        max_tries = 5
        count = 0
        if wait_for_ping:
            while not self.ping_server():
                time.sleep(0.2)
                count = count + 1
                if count == max_tries:
                    raise RuntimeError("Failed to connect to server")

    def ping_server(self):
        ping_cmd = self.web_client.get_cmd(None, "ping")
        try:
            self.run_ops([ping_cmd])
            return True
        except:
            return False
    
    def stop_server(self):
        self.server_process.kill()

    def login(self):
        login_cmd = self.web_client.post_cmd("login", f"user={self.user_name}&password={self.password}")
        response = self.run_ops([login_cmd])
        self.token = json.loads(self.string_from_shell_result(response))["tokens"][0]["value"]
    
