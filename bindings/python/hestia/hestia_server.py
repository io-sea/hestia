import sys
import os
import multiprocessing
import urllib.request

import hestia
import time

_CONTROLLER_WITH_STORAGE_CONFIG = {"server" : {"controller" : True,
                                               "tag" : "controller0",
                                               "host" : "127.0.0.1",
                                               "port" : "8000"},
                                    "tiers" : [{"name" : "0"},
                                               {"name" : "1"},
                                               {"name" : "2"},
                                               {"name" : "3"},
                                               {"name" : "4"},
                                               ],
                                    "object_store_backends" : [{"backend_type" : "file_hsm",
                                                                "tier_names" : ["0", "1", "2", "3", "4"],
                                                                "config" : {"root" : "hsm_object_store_controller0"}}],
                                    "event_feed" : {"active" : True},       
                                    "logger" : {"log_prefix" : "hestia_controller",
                                                "log_path" : "hestia_controller"}             
                                   }

class HestiaServerBase():
    def __init__(self, host: str = "127.0.0.1", port: int = 8000, config = None, blocking=True):
        self.lib = None
        self.host = host
        self.port = port
        self.config = config
        self.blocking=blocking

class HestiaServerWrapper():

    def __init__(self) -> None:
        self.host = "127.0.0.1"
        self.port = 8000
        self.controller_proc = None

    def start_controller_with_storage(self):
        self.controller_proc = multiprocessing.Process(target=self.internal_start_controller_with_storage)
        self.controller_proc.start()

        connected = False
        while not connected:
            time.sleep(0.2)
            req = urllib.request.Request("http://" + self.host + ":" + str(self.port) + "/api/v1/ping")
            try:
                urllib.request.urlopen(req)
                connected = True
            except urllib.error.HTTPError as e:
                continue

    def internal_start_controller_with_storage(self):
        controller = hestia.HestiaServer(self.host, self.port, hestia.hestia_server._CONTROLLER_WITH_STORAGE_CONFIG)
        
    def stop_controller(self):
        if self.controller_proc is not None and self.controller_proc.is_alive():
            self.controller_proc.terminate()


    

