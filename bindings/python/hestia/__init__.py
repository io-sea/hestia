import hestia.hestia_lib as hestia_lib

import uuid
import json

working_lib = hestia_lib.HestiaLib()
working_lib.load_library()

class HestiaClient():

    def __init__(self):
        self.lib = working_lib
        self.config = None
        self.lib.hestia_initialize(self.config)

    def __del__(self):
        self.lib.hestia_finish()

    def object_create(self) -> json:
        json_ret = json.loads(self.lib.hestia_create().decode("utf-8"))
        return json_ret
    
    def object_put(self, id : str, 
                   buffer: bytes, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_object_put(id, buffer, offset, tier)

    def object_get(self, id : str, 
                   length: int, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_object_get(id, length, offset, tier)