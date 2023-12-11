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

    def __del__(self):
        try:
            self.stop_server()
        except:
            pass

        proc = self.get_process_on_port("hestia", self.port)
        if proc is not None:
            proc.kill()

    def get_config_path(self, name):
        return self.project_dir / "test_data" / f"configs/{name}.yaml"

    def start_server(self, config = None, wait_for_ping = True):
        logging.info("Starting server")

        if config is None:
            config = "hestia_controller_with_backends"

        logging.info("Using config: " + config)
        config_path = self.get_config_path(config)

        start_server_cmd = f"hestia start --config={config_path}"
        self.run_ops([start_server_cmd], wait=False)

        time.sleep(0.1)
        if wait_for_ping:
            while not self.ping_server():
                time.sleep(0.1)

    def ping_server(self):
        ping_cmd = self.web_client.get_cmd(None, "ping")
        try:
            self.run_ops([ping_cmd])
            return True
        except:
            return False
    
    def stop_server(self):
        stop_server_cmd = "hestia stop"
        self.run_ops([stop_server_cmd])

    def login(self):
        login_cmd = self.web_client.post_cmd("login", f"user={self.user_name}&password={self.password}")
        response = self.run_ops([login_cmd])
        self.token = json.loads(self.string_from_shell_result(response))["tokens"][0]["value"]
    
