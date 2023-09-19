import ctypes 

from enum import IntEnum
import json
import sys
import pathlib
import os

class HestiaItemT(IntEnum):
    HESTIA_OBJECT = 0
    HESTIA_TIER = 1
    HESTIA_DATASET = 2
    HESTIA_USER = 3
    HESTIA_NODE = 4
    HESTIA_ACTION = 5
    HESTIA_USER_METADATA = 6

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

    """This class is a low-level wrapper of the Hestia c-api (hestia.h)

    It closely matches the c-api, with type conversion and memory management via ctypes

    If the :class:`hestia.HestiaClient` doesn't have required functionality then this class can
    be use directly as a lower-level alternative. It can be accessed as an attribute of 
    :class:`hestia.HestiaClient`.
    """

    def __init__(self) -> None:
        self.lib_handle = None 

    def load_library(self):

        """
        Load the hestia library, if it fails try a hard-coded path relative to the
        default source location.
        """

        try:
            self.lib_handle = ctypes.cdll.LoadLibrary(self.get_library_name())
        except OSError as e:
            self.load_library_fallback(e)

    def load_library_fallback(self, original_exception):
        source_file_path = pathlib.Path(__file__)
        build_path = source_file_path.parent / "../../.." / self.get_library_name()
        if build_path.is_file():
            self.lib_handle = ctypes.cdll.LoadLibrary(build_path.resolve())
        else: 
            raise original_exception

    def get_library_name(self):
        extension = ".so"
        if sys.platform == "darwin":
            extension = ".dylib"
        return "libhestia" + extension

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
                                           input.encode("utf-8"), 
                                           len(input), 
                                           output_format, 
                                           ctypes.byref(output_p), 
                                           ctypes.byref(output_length))
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))

        output_copied = output_p.value
        self.lib_handle.hestia_free_output(ctypes.byref(output_p))

        return output_copied.decode("utf-8")
    
    def hestia_update(self, subject: HestiaItemT = HestiaItemT.HESTIA_OBJECT, 
                      input_format: HestiaIoFormatT = HestiaIoFormatT.HESTIA_IO_JSON, 
                      id_format: HestiaIdFormatT = HestiaIdFormatT.HESTIA_ID, 
                      input: str = "", 
                      output_format: HestiaIoFormatT = HestiaIoFormatT.HESTIA_IO_JSON):
        output_p = ctypes.c_char_p()
        output_length = ctypes.c_uint64()
        
        rc = self.lib_handle.hestia_update(subject, 
                                           input_format, 
                                           id_format, 
                                           input.encode("utf-8"), 
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

        if output_copied is not None:
            return output_copied.decode("utf-8")
        else:
            return json.dumps([])
    
    def hestia_remove(self, subject: HestiaItemT = HestiaItemT.HESTIA_OBJECT, 
                      id_format: HestiaIdFormatT = HestiaIdFormatT.HESTIA_ID, 
                      input: str = ""):

        rc = self.lib_handle.hestia_remove(subject, 
                                           id_format, 
                                           input.encode("utf-8"), 
                                           len(input))
        if rc != 0:
            raise ValueError('Error code: ' + str(rc))
    
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

        rc = self.lib_handle.hestia_data_put_descriptor(id.encode("utf-8"), fd, 
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
    

    
