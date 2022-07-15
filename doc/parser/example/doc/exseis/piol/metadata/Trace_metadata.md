# <a name='exseis-piol-metadata-Trace_metadata' /> public exseis::piol::metadata::Trace_metadata

Class for initialising the trace parameter structure and storing a structure with the necessary rules. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Entry_not_found](./Trace_metadata/Entry_not_found.md) | An exception class thrown when a metadata entry is queried which hasn't been added to this instance.  |


## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [~Trace_metadata](#exseis-piol-metadata-Trace_metadata-~Trace_metadata) | Default destructor The destructor is defined here for capture during wraptests.  |
| [Trace_metadata](#exseis-piol-metadata-Trace_metadata-Trace_metadata) | Copy constructor: default.  |
| [operator=](#exseis-piol-metadata-Trace_metadata-operator=) | Copy assignment: default.  |
| [Trace_metadata](#exseis-piol-metadata-Trace_metadata-Trace_metadata-1) | Move constructor: default.  |
| [operator=](#exseis-piol-metadata-Trace_metadata-operator=-1) | Move assignment: default.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [Key](#exseis-piol-metadata-Trace_metadata-Key) | The key representing the metadata entry.  |
| [Entry_map](#exseis-piol-metadata-Trace_metadata-Entry_map) | A map for the metadata entry key to the list of entries for each trace block.  |


## Private Attributes
| Name | Description | 
| ---- | ---- |
| [m_floating_point_entries](#exseis-piol-metadata-Trace_metadata-m_floating_point_entries) | Floating point entries.  |
| [m_integer_entries](#exseis-piol-metadata-Trace_metadata-m_integer_entries) | Integer entries.  |
| [m_index_entries](#exseis-piol-metadata-Trace_metadata-m_index_entries) | trace number array.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [raw_metadata](#exseis-piol-metadata-Trace_metadata-raw_metadata) | Storage for a file format specific copy of the raw metadata.  |
| [raw_metadata_block_size](#exseis-piol-metadata-Trace_metadata-raw_metadata_block_size) | Size of a raw metadata block.  |
| [rules](#exseis-piol-metadata-Trace_metadata-rules) | The rules which describe the indexing of the arrays.  |
| [num_traces](#exseis-piol-metadata-Trace_metadata-num_traces) | The number of sets of trace parameters.  |
| [entry_types](#exseis-piol-metadata-Trace_metadata-entry_types) | A list of the type of a given metadata entry.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Trace_metadata](#exseis-piol-metadata-Trace_metadata-Trace_metadata-2) | Allocate the basic space required to store the arrays and store the rules.  |
| [Trace_metadata](#exseis-piol-metadata-Trace_metadata-Trace_metadata-3) | Allocate the basic space required to store the arrays and store the rules.  |
| [get_floating_point](#exseis-piol-metadata-Trace_metadata-get_floating_point) | Get a floating point metadata entry.  |
| [set_floating_point](#exseis-piol-metadata-Trace_metadata-set_floating_point) | Set a floating point metadata entry.  |
| [get_integer](#exseis-piol-metadata-Trace_metadata-get_integer) | Get a integer metadata entry.  |
| [set_integer](#exseis-piol-metadata-Trace_metadata-set_integer) | Set a integer metadata entry.  |
| [get_index](#exseis-piol-metadata-Trace_metadata-get_index) | Get a index metadata entry.  |
| [set_index](#exseis-piol-metadata-Trace_metadata-set_index) | Set a index metadata entry.  |
| [copy_entries](#exseis-piol-metadata-Trace_metadata-copy_entries) | Copy the metadata entries from `source_metadata` for trace index `source_trace_index` into the metadata entries of the current instance at `trace_index`.  |
| [entry_size](#exseis-piol-metadata-Trace_metadata-entry_size) | Get the number of elements for a given metadata entry.  |
| [entry_type](#exseis-piol-metadata-Trace_metadata-entry_type) | Get the underlying type for the given metadata entry.  |
| [entry_data](#exseis-piol-metadata-Trace_metadata-entry_data) | Get the data for a given metadata entry.  |
| [entry_data](#exseis-piol-metadata-Trace_metadata-entry_data-1) | Get the data for a given metadata entry.  |
| [size](#exseis-piol-metadata-Trace_metadata-size) | Return the number of sets of trace parameters.  |
| [memory_usage](#exseis-piol-metadata-Trace_metadata-memory_usage) |  |



## Special Member Functions
### <a name='exseis-piol-metadata-Trace_metadata-~Trace_metadata' /> public  exseis::piol::metadata::Trace_metadata::~Trace_metadata ()

Default destructor The destructor is defined here for capture during wraptests. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-Trace_metadata' /> public  exseis::piol::metadata::Trace_metadata::Trace_metadata (const Trace_metadata &)=default

Copy constructor: default. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-operator=' /> public [Trace_metadata][exseis-piol-metadata-Trace_metadata] & exseis::piol::metadata::Trace_metadata::operator= (const Trace_metadata &)=default

Copy assignment: default. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_metadata][exseis-piol-metadata-Trace_metadata] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-Trace_metadata-1' /> public  exseis::piol::metadata::Trace_metadata::Trace_metadata (Trace_metadata &&)=default

Move constructor: default. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-operator=-1' /> public [Trace_metadata][exseis-piol-metadata-Trace_metadata] & exseis::piol::metadata::Trace_metadata::operator= (Trace_metadata &&)=default

Move assignment: default. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_metadata][exseis-piol-metadata-Trace_metadata] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

## Public Types
### <a name='exseis-piol-metadata-Trace_metadata-Key' /> public exseis::piol::metadata::Trace_metadata::Key 

The key representing the metadata entry. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-Entry_map' /> public exseis::piol::metadata::Trace_metadata::Entry_map 

A map for the metadata entry key to the list of entries for each trace block. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

## Private Attributes
### <a name='exseis-piol-metadata-Trace_metadata-m_floating_point_entries' /> private exseis::piol::metadata::Trace_metadata::m_floating_point_entries 

Floating point entries. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-m_integer_entries' /> private exseis::piol::metadata::Trace_metadata::m_integer_entries 

Integer entries. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-m_index_entries' /> private exseis::piol::metadata::Trace_metadata::m_index_entries 

trace number array. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

## Public Attributes
### <a name='exseis-piol-metadata-Trace_metadata-raw_metadata' /> public exseis::piol::metadata::Trace_metadata::raw_metadata 

Storage for a file format specific copy of the raw metadata. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-raw_metadata_block_size' /> public exseis::piol::metadata::Trace_metadata::raw_metadata_block_size  = 0

Size of a raw metadata block. 










Indicates how large an individual raw metadata block is for a trace block. Used for indexing the `raw_metadata` block. 




[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-rules' /> public exseis::piol::metadata::Trace_metadata::rules 

The rules which describe the indexing of the arrays. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-num_traces' /> public exseis::piol::metadata::Trace_metadata::num_traces 

The number of sets of trace parameters. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-entry_types' /> public exseis::piol::metadata::Trace_metadata::entry_types 

A list of the type of a given metadata entry. 








[Go to Top](#exseis-piol-metadata-Trace_metadata)

## Public Functions
### <a name='exseis-piol-metadata-Trace_metadata-Trace_metadata-2' /> public  exseis::piol::metadata::Trace_metadata::Trace_metadata (Rule rules, size_t num_traces)

Allocate the basic space required to store the arrays and store the rules. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | Rule | rules | The rules which describe the layout of the arrays.  |
| in | size_t | num_traces | The number of sets of trace parameters.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-Trace_metadata-3' /> public  exseis::piol::metadata::Trace_metadata::Trace_metadata (size_t num_traces=1)

Allocate the basic space required to store the arrays and store the rules. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | num_traces | The number of sets of trace parameters.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-get_floating_point' /> public exseis::utils::Floating_point exseis::piol::metadata::Trace_metadata::get_floating_point (size_t trace_index, Key entry) const

Get a floating point metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to get.  |
| in | size_t | trace_index | The index of the trace block to get the metadata for. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Floating_point | The value of the metadata indexed by `entry` for trace block `trace_index`.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-set_floating_point' /> public void exseis::piol::metadata::Trace_metadata::set_floating_point (size_t trace_index, Key entry, exseis::utils::Floating_point value)

Set a floating point metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to set.  |
| in | size_t | trace_index | The index of the trace block to set the metadata for.  |
| in | exseis::utils::Floating_point | value | The value to set the metadata entry to.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-get_integer' /> public exseis::utils::Integer exseis::piol::metadata::Trace_metadata::get_integer (size_t trace_index, Key entry) const

Get a integer metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to get.  |
| in | size_t | trace_index | The index of the trace block to get the metadata for. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Integer | The value of the metadata indexed by `entry` for trace block `trace_index`.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-set_integer' /> public void exseis::piol::metadata::Trace_metadata::set_integer (size_t trace_index, Key entry, exseis::utils::Integer value)

Set a integer metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to set.  |
| in | size_t | trace_index | The index of the trace block to set the metadata for.  |
| in | exseis::utils::Integer | value | The value to set the metadata entry to.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-get_index' /> public size_t exseis::piol::metadata::Trace_metadata::get_index (size_t trace_index, Key entry) const

Get a index metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to get.  |
| in | size_t | trace_index | The index of the trace block to get the metadata for. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The value of the metadata indexed by `entry` for trace block `trace_index`.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-set_index' /> public void exseis::piol::metadata::Trace_metadata::set_index (size_t trace_index, Key entry, size_t value)

Set a index metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry to set.  |
| in | size_t | trace_index | The index of the trace block to set the metadata for.  |
| in | size_t | value | The value to set the metadata entry to.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-copy_entries' /> public void exseis::piol::metadata::Trace_metadata::copy_entries (size_t trace_index, const Trace_metadata &source_metadata, size_t source_trace_index)

Copy the metadata entries from `source_metadata` for trace index `source_trace_index` into the metadata entries of the current instance at `trace_index`. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | trace_index | The trace index of the current instance to copy the metadata entries into.  |
| in | const [Trace_metadata][exseis-piol-metadata-Trace_metadata] & | source_metadata | The [Trace_metadata][exseis-piol-metadata-Trace_metadata] object to copy the metadata entries from.  |
| in | size_t | source_trace_index | The trace index to copy the entries of `source_metadata` from.  |












[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-entry_size' /> public size_t exseis::piol::metadata::Trace_metadata::entry_size (Key entry) const

Get the number of elements for a given metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry key. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of elements for the given metadata entry.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-entry_type' /> public exseis::utils::Type exseis::piol::metadata::Trace_metadata::entry_type (Key entry) const

Get the underlying type for the given metadata entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The metadata entry key. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Type | The `exseis::utils::Type` representing the underlying storage used for the metadata data.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-entry_data' /> public const T * exseis::piol::metadata::Trace_metadata::entry_data (Key entry) const

Get the data for a given metadata entry. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type of the metadata entry. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The key for the metadata entry. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| const T * | The underlying data for the metadata storage, as a pointer to an array of `T`.  |







The parameter `T` should match the underlying storage for the data. If they don't match, `nullptr` will be returned.

When `T` is `unsigned char`, the data will be returned as an array of `unsigned char`, regardless of the underlying storage type. This can be useful for using the data with, say, MPI, without having to know the correct types in advance.

For a metadata entry that we know is storing its data as a float: 
```c++
assert(
    trace_metadata.entry_type(my_entry)
    == exseis::utils::Type::Float);

float* data = trace_metadata.entry_data< float >(my_entry);

assert(data != nullptr);

for (size_t i = 0; i < trace_metadata.entry_size(my_entry); i++) {
    do_something(data[i]);
}
```




#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-entry_data-1' /> public T * exseis::piol::metadata::Trace_metadata::entry_data (Key entry)

Get the data for a given metadata entry. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type of the metadata entry. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key][exseis-piol-metadata-Trace_metadata_key] | entry | The key for the metadata entry. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T * | The underlying data for the metadata storage, as a pointer to an array of `T`.  |







The parameter `T` should match the underlying storage for the data. If they don't match, `nullptr` will be returned.

When `T` is `unsigned char`, the data will be returned as an array of `unsigned char`, regardless of the underlying storage type. This can be useful for using the data with, say, MPI, without having to know the correct types in advance.

For a metadata entry that we know is storing its data as a float: 
```c++
assert(
    trace_metadata.entry_type(my_entry)
    == exseis::utils::Type::Float);

float* data = trace_metadata.entry_data< float >(my_entry);

assert(data != nullptr);

for (size_t i = 0; i < trace_metadata.entry_size(my_entry); i++) {
    do_something(data[i]);
}
```




[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-size' /> public size_t exseis::piol::metadata::Trace_metadata::size () const

Return the number of sets of trace parameters. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Number of sets  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

### <a name='exseis-piol-metadata-Trace_metadata-memory_usage' /> public size_t exseis::piol::metadata::Trace_metadata::memory_usage () const




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return estimate in bytes.  |







Estimate of the total memory used




#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-Trace_metadata)

[exseis-piol-metadata-Trace_metadata]:./Trace_metadata.md
[exseis-piol-metadata-Trace_metadata-Entry_map]:./Trace_metadata.md#exseis-piol-metadata-Trace_metadata-Entry_map
[exseis-piol-metadata-Trace_metadata_key]:./index.md#exseis-piol-metadata-Trace_metadata_key
