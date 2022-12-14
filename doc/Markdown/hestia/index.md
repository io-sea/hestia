# <a name='hestia' />  hestia




## Functions
| Name | Description | 
| ---- | ---- |
| [create_object](#hestia-create_object) | Creates an object in the object store and an entry for it in the key value store.  |
| [put](#hestia-put) | Sends data to the object store.  |
| [get](#hestia-get) | Retrieves data from the object store.  |
| [set_attrs](#hestia-set_attrs) | Update an attribute for an object.  |
| [get_attrs](#hestia-get_attrs) |  |
| [list_attrs](#hestia-list_attrs) | List all available attributes and their values for a given object.  |
| [remove](#hestia-remove) | remove an object and from the system, release all storage space and delete its data from the kvs  |
| [list](#hestia-list) | List all objects on a given tier.  |



## Functions
### <a name='hestia-create_object' /> public void hestia::create_object (const struct hsm_uint &oid, struct hsm_obj &obj)

Creates an object in the object store and an entry for it in the key value store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to be created  |
| struct [hsm_obj][hestia-hsm_obj] & | obj | Object to be created  |












[Go to Top](#hestia)

### <a name='hestia-put' /> public int hestia::put (const struct hsm_uint oid, struct hsm_obj *obj, const bool is_overwrite, const void *buf, const std::size_t offset, const std::size_t length, const std::uint8_t target_tier=0)

Sends data to the object store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] | oid | ID of the object to send the data to  |
| struct [hsm_obj][hestia-hsm_obj] * | obj | Object to send the data to  |
| const bool | is_overwrite | Signifies whether we are overwriting an existing object or writing to a new one  |
| const void * | buf | Buffer to hold the data being sent to the object store  |
| const std::size_t | offset | Offset into the buffer to begin writing from (used for the multipart upload feature)  |
| const std::size_t | length | Amount of data being sent in bytes  |
| const std::uint8_t | target_tier | The tier to write the data to |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success, negative error code on error  |












[Go to Top](#hestia)

### <a name='hestia-get' /> public int hestia::get (const struct hsm_uint oid, struct hsm_obj *obj, void *buf, const std::size_t off, const std::size_t len, const std::uint8_t src_tier=0, const std::uint8_t tgt_tier=0)

Retrieves data from the object store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] | oid | ID of the object to retrieve the data from  |
| struct [hsm_obj][hestia-hsm_obj] * | obj | Object to retrieve the data from  |
| void * | buf | Buffer to store the retrieved data  |
| const std::size_t | off | Start offset for the data being read  |
| const std::size_t | len | Amount of data being retrieved in bytes  |
| const std::uint8_t | src_tier | Tier where the data is being read from  |
| const std::uint8_t | tgt_tier | Identifier of the tier to copy/move the data to |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success, negative error code on error  |












[Go to Top](#hestia)

### <a name='hestia-set_attrs' /> public int hestia::set_attrs (const struct hsm_uint &oid, const char *attrs)

Update an attribute for an object. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object we are updating attributes for  |
| const char * | attrs | JSON string of attributes we wish to add/update |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | Number of attributes updated on success, negative error code on error  |












[Go to Top](#hestia)

### <a name='hestia-get_attrs' /> public std::string hestia::get_attrs (const struct hsm_uint &oid, const char *attr_keys)




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to retrieve the attributes from  |
| const char * | attr_keys | The attributes we wish to retrieve |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::string | a string of the attributes retrieved in JSON format  |







@ brief Retrieve A list of attributes from the key-value store




[Go to Top](#hestia)

### <a name='hestia-list_attrs' /> public std::string hestia::list_attrs (const struct hsm_uint &oid)

List all available attributes and their values for a given object. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to list attributes for |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::string | JSON string of attributes for the object  |












[Go to Top](#hestia)

### <a name='hestia-remove' /> public int hestia::remove (const struct hsm_uint &oid)

remove an object and from the system, release all storage space and delete its data from the kvs 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to be removed |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success, negative error code on error  |












[Go to Top](#hestia)

### <a name='hestia-list' /> public std::vector< struct [hsm_uint][hestia-hsm_uint] > hestia::list (const std::uint8_t tier=0)

List all objects on a given tier. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const std::uint8_t | tier | Tier to list the objects on |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< struct [hsm_uint][hestia-hsm_uint] > | A vector of all found object IDs  |












[Go to Top](#hestia)

[hestia-hsm_obj]:./hsm_obj.md#hestia-hsm_obj
[hestia-hsm_uint]:./hsm_uint.md#hestia-hsm_uint
