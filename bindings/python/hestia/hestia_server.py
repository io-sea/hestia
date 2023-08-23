import sys
import os
import multiprocessing
import urllib.request

import hestia
import time

_CONTROLLER_CONFIG = {"server" : {"controller" : True,
                                               "tag" : "controller0",
                                               "host" : "127.0.0.1",
                                               "port" : "8000"},
                                    "tiers" : [{"name" : "0"},
                                               {"name" : "1"},
                                               {"name" : "2"},
                                               {"name" : "3"},
                                               {"name" : "4"},
                                               ],
                                    "event_feed" : {"active" : True},       
                                    "logger" : {"log_prefix" : "hestia_controller",
                                                "log_path" : "hestia_controller"}             
                                   }

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


_WORKER_CONFIG = {"server" : {"controller" : False,
                                               "tag" : "worker0",
                                               "controller_address" : "127.0.0.1:8000",
                                               "host" : "127.0.0.1",
                                               "port" : "8080"},
                                    "object_store_backends" : [{"backend_type" : "file_hsm",
                                                                "tier_names" : ["0", "1", "2", "3", "4"],
                                                                "config" : {"root" : "hsm_object_store_worker0"}}],   
                                    "logger" : {"log_prefix" : "hestia_worker0",
                                                "log_path" : "hestia_worker0"}             
                                   }


class HestiaServerBase():
    def __init__(self, host: str = "127.0.0.1", port: int = 8000, config = None, blocking=True):
        self.lib = None
        self.host = host
        self.port = port
        self.config = config
        self.blocking=blocking

def internal_start_controller_with_storage(host, port):
    controller = hestia.HestiaServer(host, port, hestia.hestia_server._CONTROLLER_WITH_STORAGE_CONFIG)

def internal_start_controller(host, port):
    controller = hestia.HestiaServer(host, port, hestia.hestia_server._CONTROLLER_CONFIG)

def internal_start_worker(host, port):
    worker = hestia.HestiaServer(host, port, hestia.hestia_server._WORKER_CONFIG)

class HestiaServerWrapper():

    def __init__(self) -> None:
        self.controller_host = "127.0.0.1"
        self.controller_port = 8000
        self.controller_proc = None

        self.worker_host = "127.0.0.1"
        self.worker_port = "8080"
        self.worker_proc = None

    def start_controller_with_storage(self):
        self.controller_proc = multiprocessing.Process(target=internal_start_controller_with_storage, args=(self.controller_host, self.controller_port))
        self.controller_proc.start()
        self.wait_for_connection(self.controller_host + ":" + str(self.controller_port))

    def start_controller(self):
        self.controller_proc = multiprocessing.Process(target=internal_start_controller, args=(self.controller_host, self.controller_port))
        self.controller_proc.start()
        self.wait_for_connection(self.controller_host + ":" + str(self.controller_port))

    def start_worker(self, port):
        self.worker_proc = multiprocessing.Process(target=internal_start_worker, args=(self.worker_host, port))
        self.worker_proc.start()
        self.wait_for_connection(self.worker_host + ":" + str(port))

    def wait_for_connection(self, address):
        connected = False
        while not connected:
            time.sleep(0.2)
            req = urllib.request.Request("http://" + address + "/api/v1/ping")
            try:
                urllib.request.urlopen(req)
                connected = True
            except urllib.error.HTTPError as e:
                continue
        
    def stop(self):
        if self.controller_proc is not None and self.controller_proc.is_alive():
            self.controller_proc.terminate()

        if self.worker_proc is not None and self.worker_proc.is_alive():
            self.worker_proc.terminate()


    

