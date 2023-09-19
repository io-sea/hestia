import hestia.hestia_lib as hestia_lib
import hestia.hestia_client as hestia_client
import hestia.hestia_server as hestia_server

working_lib = hestia_lib.HestiaLib()
working_lib.load_library()

class HestiaClient(hestia_client.HestiaClientBase):

    """This class allows interaction with Hestia as a client. Most methods of interest
    are defined in the base class :class:`hestia.hestia_client.HestiaClientBase`.

    This class just manages the client lifecycle via RAII.
    
    :param config_path: Optional path to a config yaml file
    :param token: A token identifying the current user. If user-management is off it is optional
    :param config: Extra config in json format - this overrides any config in an external file
    """

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

    """This class is for running a Hestia server. Most methods of interest
    are defined in the base class :class:`hestia.hestia_server.HestiaServerBase`.

    This class just manages the server lifecycle via RAII.
    
    :param host: Host address to run at
    :param port: Host port to run at
    :param config: Extra config in json format - this overrides any config in an external file
    """
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

