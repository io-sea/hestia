import hestia.hestia_lib as hestia_lib
import hestia.hestia_client as hestia_client
import hestia.hestia_server as hestia_server

working_lib = hestia_lib.HestiaLib()
working_lib.load_library()

class HestiaClient(hestia_client.HestiaClientBase):

    def __init__(self, config_path: str = None, token: str = None, config = None):
        super().__init__(config_path, token, config)
        self.lib = working_lib
        rc = self.lib.hestia_initialize(self.config_path, self.token, self.config)
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

    def __del__(self):
        rc = self.lib.hestia_finish()
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

class HestiaServer(hestia_server.HestiaServerBase):

    def __init__(self, host: str = "127.0.0.1", port: int = 8000, config = None):
        super().__init__(host, port, config)
        self.lib = working_lib
        rc = self.lib.hestia_server_start(self.host, self.port, self.config, self.blocking)
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

    def __del__(self):
        rc = self.lib.hestia_server_stop()
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

