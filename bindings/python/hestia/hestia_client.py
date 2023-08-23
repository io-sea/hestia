import json

import hestia.hestia_lib as hestia_lib

_CLIENT_FULL_CONFIG = {"server" : {"controller_address" : "127.0.0.1:8000"}}

class HestiaClientBase():

    def __init__(self, config_path: str = None, token: str = None, config = None):
        self.lib = None
        self.config_path = config_path
        self.token = token
        self.config = config

    def object_create(self) -> json:
        return json.loads(self.lib.hestia_create())
    
    def action_read(self, id : str):
        return json.loads(self.lib.hestia_read(
            hestia_lib.HestiaItemT.HESTIA_ACTION,
            hestia_lib.HestiaQueryFormatT.HESTIA_QUERY_IDS,
            0,
            0,
            id
        ))
    
    def object_read(self) -> json:
        return json.loads(self.lib.hestia_read())
    
    def object_put(self, id : str, 
                   buffer: bytes, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put(id, buffer, offset, tier)
    
    def object_put_fd(self, id : str, 
                   fd: int, 
                   length: int,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put_fd(id, fd, length, offset, tier)
    
    def object_put_path(self, id : str, 
                   path: str, 
                   length: int = 0,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put_path(id, path, length, offset, tier)

    def object_get(self, id : str, 
                   length: int, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get(id, length, offset, tier)
    
    def object_get_fd(self, id : str, 
                   fd: int, 
                   length: int,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get_fd(id, fd, length, offset, tier)
    
    def object_get_path(self, id : str, 
                   path: str, 
                   length: int = 0,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get_path(id, path, length, offset, tier)