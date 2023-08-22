import ctypes 

from enum import IntEnum

class HestiaItemT(IntEnum):
    HESTIA_OBJECT = 0
    HESTIA_TIER = 1
    HESTIA_DATASET = 2
    HESTIA_USER = 3
    HESTIA_NODE = 4

class HestiaIoFormatT(IntEnum):
    HESTIA_IO_NONE = 0
    HESTIA_IO_IDS = 1
    HESTIA_IO_JSON = 2
    HESTIA_IO_KEY_VALUE = 4

class HestiaIdFormatT(IntEnum):
    HESTIA_ID_NONE = 0
    HESTIA_ID = 1
    HESTIA_NAME = 2
    HESTIA_PARENT_ID = 4
    HESTIA_PARENT_NAME = 8

class HestiaQueryFormatT(IntEnum):
    HESTIA_QUERY_NONE = 0
    HESTIA_QUERY_IDS = 1
    HESTIA_QUERY_FILTER = 2

class HestiaLib():

    def __init__(self) -> None:
        self.lib_handle = None 

    def load_library(self):
        self.lib_handle = ctypes.cdll.LoadLibrary("libhestia_lib.dylib")

    def hestia_initialize(self, config_path: str = None, token: str = None, extra_config: str = None) -> int:
        return self.lib_handle.hestia_initialize(config_path, token, extra_config)

    def hestia_finish(self) -> int:
        return self.lib_handle.hestia_finish()

    def hestia_create(self, subject: HestiaItemT = HestiaItemT.HESTIA_OBJECT, 
                      input_format: HestiaIoFormatT = HestiaIoFormatT.HESTIA_IO_NONE, 
                      id_format: HestiaIdFormatT = HestiaIdFormatT.HESTIA_ID, 
                      input: str = "", 
                      output_format: HestiaIoFormatT = HestiaIoFormatT.HESTIA_IO_JSON):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()
        
        rc = self.lib_handle.hestia_create(subject, 
                                           input_format, 
                                           id_format, 
                                           input, 
                                           len(input), 
                                           output_format, 
                                           ctypes.byref(output_p), 
                                           ctypes.byref(output_length))
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied

    def hestia_read(self, subject: HestiaItemT = HestiaItemT.HESTIA_OBJECT, 
                      query_format = HestiaQueryFormatT.HESTIA_QUERY_NONE,
                      id_format: HestiaIdFormatT = HestiaIdFormatT.HESTIA_ID, 
                      offset: int = 0,
                      count: int = 0,
                      input: str = "", 
                      output_format: HestiaIoFormatT = HestiaIoFormatT.HESTIA_IO_JSON):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        total_count = ctypes.c_uint64()
        
        rc = self.lib_handle.hestia_read(subject, 
                                           query_format, 
                                           id_format, 
                                           offset,
                                           count,
                                           input, 
                                           len(input), 
                                           output_format, 
                                           ctypes.byref(output_p), 
                                           ctypes.byref(output_length),
                                           ctypes.byref(total_count))
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied
    
    def hestia_data_put(self, 
                        id: str, 
                        buffer: bytes, 
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_put(id, buffer, 
                                            len(buffer), 
                                            offset, tier,
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied
    
    def hestia_data_put_fd(self, 
                        id: str, 
                        fd: int,
                        length: int, 
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_put_descriptor(id, fd, 
                                                length, 
                                                offset, tier,
                                                ctypes.byref(output_p), 
                                                ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied

    def hestia_data_get(self, 
                          id: bytes, 
                          length: int, 
                          offset: int = 0, 
                          tier: int = 0):
        
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        buffer_p = ctypes.create_string_buffer(length)
        rc = self.lib_handle.hestia_data_get(id, buffer_p, 
                                        length, 
                                        offset, 
                                        tier,
                                        output_p,
                                        output_length
                                        )
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return buffer_p.value