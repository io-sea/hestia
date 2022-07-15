# <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry' /> public exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry

The Index rule entry structure. For indexing without modifying what will be stored. 




## Inheritance:
Inherits from [exseis::piol::metadata::rules::Rule_entry][exseis-piol-metadata-rules-Rule_entry].

## Public Functions
| Name | Description | 
| ---- | ---- |
| [Segy_index_rule_entry](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-Segy_index_rule_entry) | The constructor.  |
| [min](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-min) | Return 0. nothing stored.  |
| [max](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-max) | Return 0. nothing stored.  |
| [type](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-type) | Return the datatype associated with the entry.  |
| [clone](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-clone) | Return a duplicate of the RuleEntry.  |
| [memory_usage](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-memory_usage) | Query the amount of memory used by the current instance.  |



## Public Functions
### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-Segy_index_rule_entry' /> public  exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::Segy_index_rule_entry ()

The constructor. 








#### Qualifiers: 
* inline


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-min' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::min () const override

Return 0. nothing stored. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return 0  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-max' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::max () const override

Return 0. nothing stored. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return 0  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-type' /> public [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::type () const override

Return the datatype associated with the entry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [MdType][exseis-piol-metadata-rules-Rule_entry-MdType] | [`MdType::Index`][exseis-piol-metadata-rules-Rule_entry-Index]  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-clone' /> public std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::clone () const override

Return a duplicate of the RuleEntry. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< [Rule_entry][exseis-piol-metadata-rules-Rule_entry] > | A pointer to a duplicate of the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

### <a name='exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry-memory_usage' /> public size_t exseis::piol::metadata::rules::segy_rule_entry::Segy_index_rule_entry::memory_usage () const override

Query the amount of memory used by the current instance. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The amount of memory used by the current instance.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-piol-metadata-rules-segy_rule_entry-Segy_index_rule_entry)

[exseis-piol-metadata-rules-Rule_entry]:./../Rule_entry.md
[exseis-piol-metadata-rules-Rule_entry-Index]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
[exseis-piol-metadata-rules-Rule_entry-MdType]:./../Rule_entry.md#exseis-piol-metadata-rules-Rule_entry-MdType
