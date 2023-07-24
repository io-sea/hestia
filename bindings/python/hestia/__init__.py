import hestia.hestia_lib as hestia_lib

import uuid

working_lib = hestia_lib.HestiaLib()
working_lib.load_library()

class HestiaClient():

    def __init__(self):
        self.lib = working_lib
        self.config = None
        self.lib.hestia_initialize(self.config)

    def __del__(self):
        self.lib.hestia_finish()

    def object_create(self) -> uuid.UUID:
        return uuid.UUID("{" + self.lib.hestia_object_create().decode("utf-8") + "}")
    
    def object_put(self, id : uuid.UUID, buffer: bytes, offset: int = 0, tier: int = 0):
        self.lib.hestia_object_put(str(id).encode('utf-8'), buffer, offset, tier)

    def object_get(self, id : uuid.UUID, length: int, offset: int = 0, tier: int = 0):
        return self.lib.hestia_object_get(str(id).encode('utf-8'), length, offset, tier)