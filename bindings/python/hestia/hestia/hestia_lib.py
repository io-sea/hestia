import ctypes 

from enum import IntEnum
import json

class HestiaItemT(IntEnum):
    HESTIA_OBJECT = 0
    HESTIA_TIER = 1
    HESTIA_DATASET = 2
    HESTIA_USER = 3
    HESTIA_NODE = 4
    HESTIA_ACTION = 5

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

    def hestia_initialize(self, config_path: str = None, token: str = None, extra_config = None) -> int:
        config_str = json.dumps(extra_config).encode('utf-8') if extra_config is not None else None
        return self.lib_handle.hestia_initialize(config_path, token, config_str)

    def hestia_finish(self) -> int:
        return self.lib_handle.hestia_finish()
    
    def hestia_server_start(self, host: str = "127.0.0.1", port: int = 8000, config = None, blocking = True) -> int:
        if not blocking:
            if config is None:
                config = {"server" : {"run_blocking" : False}}
            else:
                if "server" in config:
                    config["server"]["run_blocking"] = False
                else:
                    config["server"] = {"run_blocking" : False}

        config_str = json.dumps(config).encode('utf-8') if config is not None else None
        return self.lib_handle.hestia_start_server(host, port, config_str)
    
    def hestia_server_stop(self) -> int:
        return self.lib_handle.hestia_stop_server()
        
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

        return output_copied.decode("utf-8")

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
                                           input.encode("utf-8"), 
                                           len(input), 
                                           output_format, 
                                           ctypes.byref(output_p), 
                                           ctypes.byref(output_length),
                                           ctypes.byref(total_count))
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied.decode("utf-8")
    
    def hestia_data_put(self, 
                        id: str, 
                        buffer: bytes, 
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_put(id.encode('utf-8'), buffer, 
                                            len(buffer), 
                                            offset, tier,
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied.decode("utf-8")
    
    def hestia_data_put_path(self, 
                        id: str, 
                        path: str, 
                        length: int = 0,
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_put_path(id.encode('utf-8'), path.encode('utf-8'), 
                                            length,
                                            offset, tier,
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied.decode("utf-8")
    
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

        return output_copied.decode("utf-8")

    def hestia_data_get(self, 
                          id: bytes, 
                          length: int, 
                          offset: int = 0, 
                          tier: int = 0):
        
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()
        input_length = ctypes.c_uint64(length)

        buffer_p = ctypes.create_string_buffer(length)
        rc = self.lib_handle.hestia_data_get(id.encode('utf-8'), buffer_p, 
                                        ctypes.byref(input_length), 
                                        offset, 
                                        tier,
                                        ctypes.byref(output_p),
                                        ctypes.byref(output_length)
                                        )
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return buffer_p.value if length == len(buffer_p.value) else buffer_p.value[0:length]
    
    def hestia_data_get_fd(self, 
                        id: str, 
                        fd: int,
                        length: int, 
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_get_descriptor(id.encode('utf-8'), fd, 
                                                length, 
                                                offset, tier,
                                                ctypes.byref(output_p), 
                                                ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return output_copied.decode("utf-8")
    
    def hestia_data_get_path(self, 
                        id: str, 
                        path: str, 
                        length: int = 0,
                        offset: int = 0, 
                        tier: int = 0):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_get_path(id.encode('utf-8'), path.encode('utf-8'), 
                                            length,
                                            offset, tier,
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return output_copied.decode("utf-8")
    
    def hestia_data_copy(self, 
                        id: str, 
                        source_tier: int,
                        target_tier: int):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_copy(HestiaItemT.HESTIA_OBJECT, id.encode('utf-8'), 
                                            source_tier, 
                                            target_tier, 
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return output_copied.decode("utf-8")
    
    def hestia_data_move(self, 
                        id: str, 
                        source_tier: int,
                        target_tier: int):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_move(HestiaItemT.HESTIA_OBJECT, id.encode('utf-8'), 
                                            source_tier, 
                                            target_tier, 
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return output_copied.decode("utf-8")
    
    def hestia_data_release(self, 
                            id: str, 
                            tier: int):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()

        rc = self.lib_handle.hestia_data_release(HestiaItemT.HESTIA_OBJECT, id.encode('utf-8'), 
                                            tier, 
                                            ctypes.byref(output_p), 
                                            ctypes.byref(output_length))
        
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))
        return output_copied.decode("utf-8")
    

    
