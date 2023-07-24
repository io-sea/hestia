import ctypes 

class HestiaLib():

    def __init__(self) -> None:
        self.lib_handle = None 

    def load_library(self):
        self.lib_handle = ctypes.cdll.LoadLibrary("libhestia_lib.dylib")

    def hestia_initialize(self, config):
        self.lib_handle.hestia_initialize(config)

    def hestia_finish(self):
        self.lib_handle.hestia_finish()

    def hestia_object_create(self):
        p = ctypes.create_string_buffer(37)
        self.lib_handle.hestia_object_create(p)
        return p.value
    
    def hestia_object_put(self, id: bytes, buffer: bytes, offset: int, tier: int):
        self.lib_handle.hestia_object_put(id, buffer, offset, len(buffer), tier)

    def hestia_object_get(self, id: bytes, length: int, offset: int, tier: int):
        p = ctypes.create_string_buffer(length)
        self.lib_handle.hestia_object_get(id, p, offset, length, tier)
        return p.value