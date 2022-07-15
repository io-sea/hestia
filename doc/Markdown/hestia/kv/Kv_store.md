# <a name='hestia-kv-Kv_store' /> public hestia::kv::Kv_store

virtual key value store class to be implemented by key value store solutions 




## Public Functions
| Name | Description | 
| ---- | ---- |
| [Kv_store](#hestia-kv-Kv_store-Kv_store) | default constructor  |
| [~Kv_store](#hestia-kv-Kv_store-~Kv_store) | default destructor  |
| [object_exists](#hestia-kv-Kv_store-object_exists) | check if an object exists  |
| [put_meta_data](#hestia-kv-Kv_store-put_meta_data) | Send the metadata associated with an object to the key-value store.  |
| [put_meta_data](#hestia-kv-Kv_store-put_meta_data-1) | Send the metadata associated with an object to the key-value store.  |
| [get_meta_data](#hestia-kv-Kv_store-get_meta_data) | Retrieve the metadata associated with an object from the key-value store and populate the object with it.  |
| [get_meta_data](#hestia-kv-Kv_store-get_meta_data-1) | Retrieve all entries of the metadata associated with an object.  |
| [get_meta_data](#hestia-kv-Kv_store-get_meta_data-2) | Retrieve entries of the metadata associated with an object, corresponding to the given keys.  |
| [remove](#hestia-kv-Kv_store-remove) | Remove an object from the KV_store.  |
| [list](#hestia-kv-Kv_store-list) | List.  |



## Public Functions
### <a name='hestia-kv-Kv_store-Kv_store' /> public  hestia::kv::Kv_store::Kv_store ()

default constructor 








#### Qualifiers: 
* inline


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-~Kv_store' /> public  hestia::kv::Kv_store::~Kv_store ()

default destructor 








#### Qualifiers: 
* inline
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-object_exists' /> public bool hestia::kv::Kv_store::object_exists (const struct hsm_uint &oid)=0

check if an object exists 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to check for existence |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | boolean for whether the object exists  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-put_meta_data' /> public int hestia::kv::Kv_store::put_meta_data (const struct hsm_obj &obj)=0

Send the metadata associated with an object to the key-value store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_obj][hestia-hsm_obj] & | obj | Object to send the metadata to |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success or error code on failure  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-put_meta_data-1' /> public int hestia::kv::Kv_store::put_meta_data (const struct hsm_uint &oid, const nlohmann::json &attrs)=0

Send the metadata associated with an object to the key-value store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to set attributes for  |
| const nlohmann::json & | attrs | JSON string representing the attributes to set |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success or error code on failure  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-get_meta_data' /> public int hestia::kv::Kv_store::get_meta_data (struct hsm_obj &obj)=0

Retrieve the metadata associated with an object from the key-value store and populate the object with it. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| struct [hsm_obj][hestia-hsm_obj] & | obj | Object to retrieve the metadata for |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success or error code on failure  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-get_meta_data-1' /> public nlohmann::json hestia::kv::Kv_store::get_meta_data (const struct hsm_uint &oid)=0

Retrieve all entries of the metadata associated with an object. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to retrieve attributed for |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| nlohmann::json | JSON string to hold the retrieved attributes  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-get_meta_data-2' /> public nlohmann::json hestia::kv::Kv_store::get_meta_data (const struct hsm_uint &oid, const std::string &attr_keys)=0

Retrieve entries of the metadata associated with an object, corresponding to the given keys. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to retrieve attributed for |
| const std::string & | attr_keys | List of the keys we want to retrieve values for |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| nlohmann::json | JSON string to hold the retrieved attributes  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-remove' /> public int hestia::kv::Kv_store::remove (const struct hsm_uint &oid)=0

Remove an object from the KV_store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to be removed |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | 0 on success or error code on failure  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

### <a name='hestia-kv-Kv_store-list' /> public std::vector< struct [hsm_uint][hestia-hsm_uint] > hestia::kv::Kv_store::list (const std::uint8_t tier=0)=0

List. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const std::uint8_t | tier | Storage tier from which to list objects |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< struct [hsm_uint][hestia-hsm_uint] > | array of IDs for the objects found  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-kv-Kv_store)

[hestia-hsm_obj]:./../hsm_obj.md#hestia-hsm_obj
[hestia-hsm_uint]:./../hsm_uint.md#hestia-hsm_uint
