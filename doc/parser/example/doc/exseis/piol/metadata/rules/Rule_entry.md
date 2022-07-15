# <a name='exseis-piol-metadata-rules-Rule_entry' /> public exseis::piol::metadata::rules::Rule_entry

An instantiation of this structure corresponds to a single metadata rule. 




## Inheritance:
Is inherited by [exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry][exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry], [exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry][exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry], [exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry][exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry], [exseis::piol::metadata::rules::segy_rule_entry::Segy_long_rule_entry][exseis-piol-metadata-rules-segy_rule_entry-Segy_long_rule_entry], [exseis::piol::metadata::rules::segy_rule_entry::Segy_short_rule_entry][exseis-piol-metadata-rules-segy_rule_entry-Segy_short_rule_entry].

## Public Types
| Name | Description | 
| ---- | ---- |
| [MdType](#exseis-piol-metadata-rules-Rule_entry-MdType) | The type of data.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [loc](#exseis-piol-metadata-rules-Rule_entry-loc) | The memory location for the primary data.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Rule_entry](#exseis-piol-metadata-rules-Rule_entry-Rule_entry) | The constructor for storing the rule number and location.  |
| [~Rule_entry](#exseis-piol-metadata-rules-Rule_entry-~Rule_entry) | Virtual destructor.  |
| [min](#exseis-piol-metadata-rules-Rule_entry-min) | Return the minimum location stored, in derived cases, more data can be stored than just loc.  |
| [max](#exseis-piol-metadata-rules-Rule_entry-max) | Return the maximum location stored up to, in derived cases, more data can be stored than just loc.  |
| [type](#exseis-piol-metadata-rules-Rule_entry-type) | Return the datatype.  |
| [clone](#exseis-piol-metadata-rules-Rule_entry-clone) | Return a duplicate of the RuleEntry.  |
| [memory_usage](#exseis-piol-metadata-rules-Rule_entry-memory_usage) | Query the amount of memory used by the current instance.  |



## Public Types
### <a name='exseis-piol-metadata-rules-Rule_entry-MdType' /> public exseis::piol::metadata::rules::Rule_entry::MdType

The type of data. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Long | Long int data.  |  |
| Short | Short int data.  |  |
| Float | Floating point data.  |  |
| Index | For indexing purposes.  |  |
| Copy | Copy all relevant headers. Not file format agnostic.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

## Public Attributes
### <a name='exseis-piol-metadata-rules-Rule_entry-loc' /> public exseis::piol::metadata::rules::Rule_entry::loc 

The memory location for the primary data. 








[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

## Public Functions
### <a name='exseis-piol-metadata-rules-Rule_entry-Rule_entry' /> public  exseis::piol::metadata::rules::Rule_entry::Rule_entry (size_t loc)

The constructor for storing the rule number and location. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | loc | The location of the primary data  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-~Rule_entry' /> public  exseis::piol::metadata::rules::Rule_entry::~Rule_entry ()=default

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-min' /> public size_t exseis::piol::metadata::rules::Rule_entry::min () const =0

Return the minimum location stored, in derived cases, more data can be stored than just loc. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the minimum location  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-max' /> public size_t exseis::piol::metadata::rules::Rule_entry::max () const =0

Return the maximum location stored up to, in derived cases, more data can be stored than just loc. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the maximum location  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-type' /> public [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] exseis::piol::metadata::rules::Rule_entry::type () const =0

Return the datatype. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] | The MdType associated with the derived class.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-clone' /> public std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > exseis::piol::metadata::rules::Rule_entry::clone () const =0

Return a duplicate of the RuleEntry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > | A pointer to a duplicate of the current instance.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

### <a name='exseis-piol-metadata-rules-Rule_entry-memory_usage' /> public size_t exseis::piol::metadata::rules::Rule_entry::memory_usage () const =0

Query the amount of memory used by the current instance. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The amount of memory used by the current instance.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-metadata-rules-Rule_entry)

[exseis-piol-metadata-rules-Rule_entry]:./Rule_entry.md
[exseis-piol-metadata-rules-Rule_entry-MdType]:./Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
[exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry]:./segy_rule_entry/Segy_copy_rule_entry.md#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry
[exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry]:./segy_rule_entry/Segy_float_rule_entry.md#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry
[exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry]:./segy_rule_entry/Segy_index_rule_entry.md#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry
[exseis-piol-metadata-rules-segy_rule_entry-Segy_long_rule_entry]:./segy_rule_entry/Segy_long_rule_entry.md#exseis-piol-metadata-rules-segy_rule_entry-Segy_long_rule_entry
[exseis-piol-metadata-rules-segy_rule_entry-Segy_short_rule_entry]:./segy_rule_entry/Segy_short_rule_entry.md#exseis-piol-metadata-rules-segy_rule_entry-Segy_short_rule_entry
