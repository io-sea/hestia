# <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry' /> public exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry

The Copy rule entry structure for the SEG-Y format. 




## Inheritance:
Inherits from [exseis::piol::metadata::rules::Rule_entry][exseis-piol-metadata-rules-Rule_entry].

## Public Functions
| Name | Description | 
| ---- | ---- |
| [Segy_copy_rule_entry](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-Segy_copy_rule_entry) | The constructor (empty).  |
| [min](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-min) | Return the minimum location stored, i.e 0.  |
| [max](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-max) | Return the size of the trace header.  |
| [type](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-type) | Return the datatype associated with the entry.  |
| [clone](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-clone) | Return a duplicate of the RuleEntry.  |
| [memory_usage](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-memory_usage) | Query the amount of memory used by the current instance.  |



## Public Functions
### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-Segy_copy_rule_entry' /> public  exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::Segy_copy_rule_entry ()

The constructor (empty). 








#### Qualifiers: 
* inline


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-min' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::min () const override

Return the minimum location stored, i.e 0. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | 0U  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-max' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::max () const override

Return the size of the trace header. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the size of the trace header  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-type' /> public [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::type () const override

Return the datatype associated with the entry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] | [`MdType::Copy`][exseis-piol-metadata-rules-Rule_entry-Copy]  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-clone' /> public std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::clone () const override

Return a duplicate of the RuleEntry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > | A pointer to a duplicate of the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry-memory_usage' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_copy_rule_entry::memory_usage () const override

Query the amount of memory used by the current instance. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The amount of memory used by the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_copy_rule_entry)

[exseis-piol-metadata-rules-Rule_entry]:./../Rule_entry.md
[exseis-piol-metadata-rules-Rule_entry-Copy]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
[exseis-piol-metadata-rules-Rule_entry-MdType]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
