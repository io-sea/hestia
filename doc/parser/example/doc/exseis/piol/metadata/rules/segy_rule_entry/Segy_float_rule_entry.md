# <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry' /> public exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry

The Float rule entry structure for the SEG-Y format. 




## Inheritance:
Inherits from [exseis::piol::metadata::rules::Rule_entry][exseis-piol-metadata-rules-Rule_entry].

## Public Attributes
| Name | Description | 
| ---- | ---- |
| [scalar_location](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-scalar_location) | The location of the scaler field.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Segy_float_rule_entry](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-Segy_float_rule_entry) | The constructor.  |
| [min](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-min) | Return the minimum location stored.  |
| [max](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-max) | Return the maximum location stored up to, including the size of the data stored.  |
| [type](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-type) | Return the datatype associated with the entry.  |
| [clone](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-clone) | Return a duplicate of the RuleEntry.  |
| [memory_usage](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-memory_usage) | Query the amount of memory used by the current instance.  |



## Public Attributes
### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-scalar_location' /> public exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::scalar_location 

The location of the scaler field. 








[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

## Public Functions
### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-Segy_float_rule_entry' /> public  exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::Segy_float_rule_entry (Trace_header_offsets loc, Trace_header_offsets scalar_location)

The constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | Trace_header_offsets | loc | The location of the primary data  |
| in | Trace_header_offsets | scalar_location | The location of the scaler field.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-min' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::min () const override

Return the minimum location stored. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the minimum location  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-max' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::max () const override

Return the maximum location stored up to, including the size of the data stored. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the maximum location. If the scaler is in a location higher than the the primary data store then the location + 2U is returned, otherwise the primary location + 4U is returned.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-type' /> public [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::type () const override

Return the datatype associated with the entry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] | [`MdType::Float`][exseis-piol-metadata-rules-Rule_entry-Float]  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-clone' /> public std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::clone () const override

Return a duplicate of the RuleEntry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > | A pointer to a duplicate of the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry-memory_usage' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_float_rule_entry::memory_usage () const override

Query the amount of memory used by the current instance. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The amount of memory used by the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_float_rule_entry)

[exseis-piol-metadata-rules-Rule_entry]:./../Rule_entry.md
[exseis-piol-metadata-rules-Rule_entry-Float]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
[exseis-piol-metadata-rules-Rule_entry-MdType]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
