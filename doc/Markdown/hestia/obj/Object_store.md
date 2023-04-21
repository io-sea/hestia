# <a name='hestia-obj-Object_store' /> public hestia::obj::Object_store

virtual object storage class to be implemented by backend storage solutions 




## Public Functions
| Name | Description | 
| ---- | ---- |
| [Object_store](#hestia-obj-Object_store-Object_store) | default constructor  |
| [~Object_store](#hestia-obj-Object_store-~Object_store) | default destructor  |
| [put](#hestia-obj-Object_store-put) | Send the actual data to object storage.  |
| [get](#hestia-obj-Object_store-get) | Retrieve the actual data from the object store.  |
| [remove](#hestia-obj-Object_store-remove) | Remove the object for the backend object storage.  |



## Public Functions
### <a name='hestia-obj-Object_store-Object_store' /> public  hestia::obj::Object_store::Object_store ()

default constructor 








#### Qualifiers: 
* inline


[Go to Top](#hestia-obj-Object_store)

### <a name='hestia-obj-Object_store-~Object_store' /> public  hestia::obj::Object_store::~Object_store ()

default destructor 








#### Qualifiers: 
* inline
* virtual


[Go to Top](#hestia-obj-Object_store)

### <a name='hestia-obj-Object_store-put' /> public int hestia::obj::Object_store::put (const struct hsm_uint &oid, const void *buf, const std::size_t length, const std::uint8_t target_tier=0)=0

Send the actual data to object storage. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object we are sending the data to  |
| const void * | buf | Address of the data  |
| const std::size_t | length | Amount of data to send in bytes  |
| const std::uint8_t | target_tier | Tier we are sending the data to  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int |  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-obj-Object_store)

### <a name='hestia-obj-Object_store-get' /> public int hestia::obj::Object_store::get (const struct hsm_uint &oid, void *buf, const std::size_t off, const std::size_t length, const std::uint8_t src_tier=0)=0

Retrieve the actual data from the object store. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object we are retrieving the data from  |
| void * | buf | Buffer to store the data retrieved  |
| const std::size_t | off | Offset into the object to begin reading from  |
| const std::size_t | length | Amount of data to retrieve in bytes  |
| const std::uint8_t | src_tier | Tier the data resides on  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int |  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-obj-Object_store)

### <a name='hestia-obj-Object_store-remove' /> public int hestia::obj::Object_store::remove (const struct hsm_uint &oid, const std::uint8_t tier)=0

Remove the object for the backend object storage. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const struct [hsm_uint][hestia-hsm_uint] & | oid | ID of the object to be removed  |
| const std::uint8_t | tier | the object resides on  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int |  |












#### Qualifiers: 
* virtual


[Go to Top](#hestia-obj-Object_store)

[hestia-hsm_uint]:./../hsm_uint.md#hestia-hsm_uint
