import json

import hestia.hestia_lib as hestia_lib

_CLIENT_FULL_CONFIG = {"server" : {"controller_address" : "127.0.0.1:8000"}}

class HestiaClientBase():

    """This is the primary class for interacting with Hestia as a client. 
    
    The derived :class:`hestia.HestiaClient` is a small shim for managing client lifecyle, 
    this class has all methods of practical interest.

    :param config_path: Optional path to a config yaml file
    :param token: A token identifying the current user. If user-management is off it is optional
    :param config: Extra config in json format - this overrides any config in an external file
    """

    def __init__(self, config_path: str = None, token: str = None, config = None):
        self.lib = None
        self.config_path = config_path
        self.token = token
        self.config = config

    """ Create a storage object in Hestia

    :param id: Optional id for the object - if None Hestia will create one
    :return: A JSON representation of the object
    """
    def object_create(self, id: str = None) -> json:
        if id is None:
            return json.loads(self.lib.hestia_create())
        else:
            return json.loads(self.lib.hestia_create(
                hestia_lib.HestiaItemT.HESTIA_OBJECT,
                hestia_lib.HestiaIoFormatT.HESTIA_IO_IDS,
                hestia_lib.HestiaIdFormatT.HESTIA_ID,
                id
            ))

    """ Update the SYSTEM attributes of an object

    :param object: A JSON representation of the object, must at least contain the 'id' field.
    :return: An updated JSON representation of the object
    """    
    def object_update(self, object: json) -> json:
        return json.loads(self.lib.hestia_update(input=json.dumps(object)))
    
    """ Update the USER attributes of an object

    :param id: The object identifier
    :param attrs: User specified attributes as key-value pairs
    :return: An updated JSON representation of the object
    """   
    def object_attrs_put(self, id: str, attrs: dict) -> json:
        attr_str = ""
        for key, value in attrs.items():
            attr_str += f"data.{key},{value}\n"

        payload = id + "\n\n" + attr_str

        return json.loads(self.lib.hestia_update(
            hestia_lib.HestiaItemT.HESTIA_USER_METADATA,
            hestia_lib.HestiaIoFormatT.HESTIA_IO_IDS | hestia_lib.HestiaIoFormatT.HESTIA_IO_KEY_VALUE,
            hestia_lib.HestiaIdFormatT.HESTIA_PARENT_ID,
            payload
        ))
    
    """ Read (or list) all objects

    :return: All objects in the system in json format
    """ 
    def object_read(self) -> json:
        return json.loads(self.lib.hestia_read())
    
    """ Read (or list) specific objects

    :param ids: a list of object ids
    :return: Objects corresponding to the provided ids in json form
    """ 
    def object_read_ids(self, ids: list) -> json:
        return json.loads(self.lib.hestia_read(
            hestia_lib.HestiaItemT.HESTIA_OBJECT,
            hestia_lib.HestiaQueryFormatT.HESTIA_QUERY_IDS,
            hestia_lib.HestiaIdFormatT.HESTIA_ID,
            0,
            0,
            self.flatten_ids(ids)
        ))

    """ Get USER attributes of an object

    :param id: the id of the object to query
    :return: The object Metadata (attributes)
    """ 
    def object_attrs_get(self, id: str) -> dict:
        md_item = json.loads(self.lib.hestia_read(
            hestia_lib.HestiaItemT.HESTIA_USER_METADATA,
            hestia_lib.HestiaQueryFormatT.HESTIA_QUERY_IDS,
            hestia_lib.HestiaIdFormatT.HESTIA_PARENT_ID,
            input=id, output_format=hestia_lib.HestiaIoFormatT.HESTIA_IO_JSON
        ))
        return md_item["data"]

    """ Remove and object entirely from the system. This releases the object data on all tiers also.

    :param id: the id of the object to remove
    """ 
    def object_remove(self, id: str):
        self.lib.hestia_remove(input=id)
    
    """ Add data to an object using an in-memory buffer, optionally on a specified tier

    :param id: the id of the object to add data to
    :param buffer: a byte buffer with the data to add
    :param offset: offset into the object data to insert the buffer
    :param tier: the storage tier to add the data to.
    :return: an ID for the HSM activity. Can be queried for completion status.
    """ 
    def object_data_put(self, id : str, 
                   buffer: bytes, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put(id, buffer, offset, tier)

    """ Add data to an object using an open file descriptor, optionally on a specified tier

    :param id: the id of the object to add data to
    :param fd: a file descriptor, as returned from e.g. POSIX open()
    :param length: number of bytes to read from the fd
    :param offset: offset into the object data to insert the data
    :param tier: the storage tier to add the data to.
    :return: an ID for the HSM activity. Can be queried for completion status.
    """   
    def object_data_put_fd(self, id : str, 
                   fd: int, 
                   length: int,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put_fd(id, fd, length, offset, tier)
    
    """ Add data to an object using a path to a file, optionally on a specified tier

    :param id: the id of the object to add data to
    :param fd: a path to a file on the system to read from
    :param length: number of bytes to read from the file, default will read full file
    :param offset: offset into the object data to insert the data
    :param tier: the storage tier to add the data to.
    :return: an ID for the HSM activity. Can be queried for completion status.
    """   
    def object_data_put_path(self, id : str, 
                   path: str, 
                   length: int = 0,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_put_path(id, path, length, offset, tier)

    """ Get the object data in a in-memory buffer, optionally from a specified tier

    :param id: the id of the object to get data from
    :param length: number of bytes to read from the object
    :param offset: offset into the object data to get the data
    :param tier: the storage tier to get the data from
    :return: the data as bytes
    """   
    def object_data_get(self, id : str, 
                   length: int, 
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get(id, length, offset, tier)

    """ Get the object data and write to an open file descriptor, optionally from a specified tier

    :param id: the id of the object to get data from
    :param fd: an open file descriptor to write to
    :param length: number of bytes to read from the object
    :param offset: offset into the object data to get the data
    :param tier: the storage tier to get the data from
    :return: an activity id for the get operation
    """   
    def object_data_get_fd(self, id : str, 
                   fd: int, 
                   length: int,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get_fd(id, fd, length, offset, tier)

    """ Get the object data and write to a path, optionally from a specified tier

    :param id: the id of the object to get data from
    :param path: the path to write to
    :param length: number of bytes to read from the object, if empty read the full object
    :param offset: offset into the object data to get the data
    :param tier: the storage tier to get the data from
    :return: an activity id for the get operation
    """  
    def object_data_get_path(self, id : str, 
                   path: str, 
                   length: int = 0,
                   offset: int = 0, 
                   tier: int = 0):
        return self.lib.hestia_data_get_path(id, path, length, offset, tier)
    
    """ Copy object data between storage tiers

    :param id: the id of the object to copy
    :param source_tier: the tier to copy from
    :param target_tier: the tier to copy to
    :return: an activity id for the copy operation
    """  
    def object_data_copy(self, id : str, 
                    source_tier: int,
                    target_tier: int):
        return self.lib.hestia_data_copy(id, source_tier, target_tier)
    
    """ Move object data between storage tiers - this is a Copy followed by Release from the source tier

    :param id: the id of the object to move
    :param source_tier: the tier to move from
    :param target_tier: the tier to move to
    :return: an activity id for the move operation
    """  
    def object_data_move(self, id : str, 
                    source_tier: int,
                    target_tier: int):
        return self.lib.hestia_data_move(id, source_tier, target_tier)
    
    """ Release an object from a storage tier

    :param id: the id of the object to release
    :param tier: the tier to release from
    :return: an activity id for the release operation
    """  
    def object_data_release(self, id : str, tier: int):
        return self.lib.hestia_data_release(id, tier)
    
    """ Read an activity using its id, the activity will be returned as json

    :param id: the id of the activity to read
    :return: an activity as json
    """  
    def action_read(self, id: str) -> json:
        return json.loads(self.lib.hestia_read(hestia_lib.HestiaItemT.HESTIA_ACTION))
    
    """ Batch read activities using their ids, they will be returned as a single json

    :param id: the ids of the activities to read
    :return: the activities as json
    """ 
    def action_read_ids(self, ids: list) -> json:
        return json.loads(self.lib.hestia_read(
            hestia_lib.HestiaItemT.HESTIA_ACTION,
            hestia_lib.HestiaQueryFormatT.HESTIA_QUERY_IDS,
            hestia_lib.HestiaIdFormatT.HESTIA_ID,
            0,
            0,
            self.flatten_ids(ids)
        ))
    
    """ Helper method to convert a user friendly list of ids into a flat format for the Hestia internal API
    """
    def flatten_ids(self, ids: list) -> str:
        id_str = ""
        for id in ids:
            id_str += id + "\n"
        if id_str:
            id_str = id_str[0:len(id_str)-1]
        return id_str