# <a name='exseis-piol-metadata-rules-Rule' /> public exseis::piol::metadata::rules::Rule

Rules associated with trace parameters. 



The structure which holds the rules associated with the trace parameters in a file. These rules describe how to interpret the metadata and also how to index the parameter structure of arrays. 




## Classes
| Name | Description | 
| ---- | ---- |
| [State_flags](./Rule/State_flags.md) | A structure for storing the state of the extent and header buffer processing.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [Rule_entry_map](#exseis-piol-metadata-rules-Rule-Rule_entry_map) | The type of the unordered map which stores all current rules. A map ensures there are no duplicates.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [num_copy](#exseis-piol-metadata-rules-Rule-num_copy) | Number of copy rules. either 0 or 1.  |
| [start](#exseis-piol-metadata-rules-Rule-start) | The starting byte position in the SEG-Y header.  |
| [end](#exseis-piol-metadata-rules-Rule-end) | The end byte position (+ 1) in the SEG-Y header.  |
| [flag](#exseis-piol-metadata-rules-Rule-flag) | The [State_flags][exseis-piol-metadata-rules-Rule-State_flags] instance for the 
[Rule][exseis-piol-metadata-rules-Rule] instance.  |
| [rule_entry_map](#exseis-piol-metadata-rules-Rule-rule_entry_map) | The map storing all the current rules.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Rule](#exseis-piol-metadata-rules-Rule-Rule) | The copy constructor.  |
| [operator=](#exseis-piol-metadata-rules-Rule-operator=) | The copy assignment operator.  |
| [operator==](#exseis-piol-metadata-rules-Rule-operator==) | Test equality of two rules.  |
| [Rule](#exseis-piol-metadata-rules-Rule-Rule-1) | The constructor for creating a [Rule][exseis-piol-metadata-rules-Rule] structure with default rules in place or no rules in place.  |
| [Rule](#exseis-piol-metadata-rules-Rule-Rule-2) | The constructor for supplying a list of Meta entries which have default locations associated with them.  |
| [Rule](#exseis-piol-metadata-rules-Rule-Rule-3) | The constructor for creating a [Rule][exseis-piol-metadata-rules-Rule] structure with default rules in place or no rules in place.  |
| [~Rule](#exseis-piol-metadata-rules-Rule-~Rule) | The destructor. Deallocates the memory associated with the rule entries.  |
| [add_rule](#exseis-piol-metadata-rules-Rule-add_rule) | Add a pre-defined rule.  |
| [add_rule](#exseis-piol-metadata-rules-Rule-add_rule-1) | Add all rules from the given argument.  |
| [add_long](#exseis-piol-metadata-rules-Rule-add_long) | Add a rule for longs.  |
| [add_segy_float](#exseis-piol-metadata-rules-Rule-add_segy_float) | Add a rule for floats.  |
| [add_short](#exseis-piol-metadata-rules-Rule-add_short) | Add a rule for floats.  |
| [add_index](#exseis-piol-metadata-rules-Rule-add_index) | Add a rule for an index.  |
| [add_copy](#exseis-piol-metadata-rules-Rule-add_copy) | Add a rule to buffer the original trace header.  |
| [rm_rule](#exseis-piol-metadata-rules-Rule-rm_rule) | Remove a rule based on the meta entry.  |
| [extent](#exseis-piol-metadata-rules-Rule-extent) | Return the size of the buffer space required for the metadata items when converting to SEG-Y.  |
| [extent](#exseis-piol-metadata-rules-Rule-extent-1) | Return the size of the buffer space required for the metadata items when converting to SEG-Y.  |
| [memory_usage](#exseis-piol-metadata-rules-Rule-memory_usage) | Estimate of the total memory used.  |
| [memory_usage_per_header](#exseis-piol-metadata-rules-Rule-memory_usage_per_header) | How much memory will each set of parameters require?  |
| [get_entry](#exseis-piol-metadata-rules-Rule-get_entry) | Get the rule entry associated with a particular meta entry.  |



## Public Types
### <a name='exseis-piol-metadata-rules-Rule-Rule_entry_map' /> public exseis::piol::metadata::rules::Rule::Rule_entry_map 

The type of the unordered map which stores all current rules. A map ensures there are no duplicates. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

## Public Attributes
### <a name='exseis-piol-metadata-rules-Rule-num_copy' /> public exseis::piol::metadata::rules::Rule::num_copy  = 0

Number of copy rules. either 0 or 1. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-start' /> public exseis::piol::metadata::rules::Rule::start  = 0

The starting byte position in the SEG-Y header. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-end' /> public exseis::piol::metadata::rules::Rule::end  = 0

The end byte position (+ 1) in the SEG-Y header. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-flag' /> public exseis::piol::metadata::rules::Rule::flag 

The [State_flags][exseis-piol-metadata-rules-Rule-State_flags] instance for the 
[Rule][exseis-piol-metadata-rules-Rule] instance. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-rule_entry_map' /> public exseis::piol::metadata::rules::Rule::rule_entry_map 

The map storing all the current rules. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

## Public Functions
### <a name='exseis-piol-metadata-rules-Rule-Rule' /> public  exseis::piol::metadata::rules::Rule::Rule (const Rule &rule)

The copy constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Rule][exseis-piol-metadata-rules-Rule] & | rule | The [Rule][exseis-piol-metadata-rules-Rule] to copy.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-operator=' /> public [Rule][exseis-piol-metadata-rules-Rule] & exseis::piol::metadata::rules::Rule::operator= (const Rule &rhs)

The copy assignment operator. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Rule][exseis-piol-metadata-rules-Rule] & | rhs | The [Rule][exseis-piol-metadata-rules-Rule] to copy. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Rule][exseis-piol-metadata-rules-Rule] & | The current instance, set to the same value as `rhs`.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-operator==' /> public bool exseis::piol::metadata::rules::Rule::operator== (const Rule &rhs) const

Test equality of two rules. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Rule][exseis-piol-metadata-rules-Rule] & | rhs | The value to test against this. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | True if equal, false otherwise.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-Rule-1' /> public  exseis::piol::metadata::rules::Rule::Rule (bool full, bool defaults, bool extras=false)

The constructor for creating a [Rule][exseis-piol-metadata-rules-Rule] structure with default rules in place or no rules in place. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | bool | full | Whether the extents are set to the default size or calculated dynamically.  |
| in | bool | defaults | Whether the default SEG-Y rules should be set.  |
| in | bool | extras | Whether maximum amount of rules should be set. Useful when copying files through the library.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-Rule-2' /> public  exseis::piol::metadata::rules::Rule::Rule (const std::vector< Trace_metadata_key > &m, bool full=true, bool defaults=false, bool extras=false)

The constructor for supplying a list of Meta entries which have default locations associated with them. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::vector< [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] > & | m | A list of meta entries with default entries. Entries without defaults will be ignored.  |
| in | bool | full | Whether the extents are set to the default size or calculated dynamically.  |
| in | bool | defaults | Whether the default SEG-Y rules should be set.  |
| in | bool | extras | Whether maximum amount of rules should be set. Useful when copying files through the library.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-Rule-3' /> public  exseis::piol::metadata::rules::Rule::Rule (const Rule_entry_map &rule_entry_map, bool full=true)

The constructor for creating a [Rule][exseis-piol-metadata-rules-Rule] structure with default rules in place or no rules in place. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Rule_entry_map][exseis-piol-metadata-rules-Rule-Rule_entry_map] & | rule_entry_map | An unordered map to initialise the internal rule_entry_map object with.  |
| in | bool | full | Whether the extents are set to the default size or calculated dynamically.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-~Rule' /> public  exseis::piol::metadata::rules::Rule::~Rule ()

The destructor. Deallocates the memory associated with the rule entries. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_rule' /> public bool exseis::piol::metadata::rules::Rule::add_rule (Trace_metadata_key m)

Add a pre-defined rule. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The Meta entry. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Return true if the rule was added, otherwise false  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_rule-1' /> public bool exseis::piol::metadata::rules::Rule::add_rule (const Rule &r)

Add all rules from the given argument. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Rule][exseis-piol-metadata-rules-Rule] & | r | Another rule pointer. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Return true if no errors  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_long' /> public void exseis::piol::metadata::rules::Rule::add_long (Trace_metadata_key m, Trace_header_offsets loc)

Add a rule for longs. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The Meta entry.  |
| in | Trace_header_offsets | loc | The location in the SEG-Y Trace Metadata (4 bytes).  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_segy_float' /> public void exseis::piol::metadata::rules::Rule::add_segy_float (Trace_metadata_key m, Trace_header_offsets loc, Trace_header_offsets scalar_location)

Add a rule for floats. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The Meta entry.  |
| in | Trace_header_offsets | loc | The location in the SEG-Y Trace Metadata for the primary data to be stored (4 bytes).  |
| in | Trace_header_offsets | scalar_location | The location in the SEG-Y Trace Metadata for the scaler to be stored (2 bytes).  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_short' /> public void exseis::piol::metadata::rules::Rule::add_short (Trace_metadata_key m, Trace_header_offsets loc)

Add a rule for floats. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The Meta entry.  |
| in | Trace_header_offsets | loc | The location in the SEG-Y Trace Metadata for the primary data to be stored (2 bytes).  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_index' /> public void exseis::piol::metadata::rules::Rule::add_index (Trace_metadata_key m)

Add a rule for an index. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The Meta entry.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-add_copy' /> public void exseis::piol::metadata::rules::Rule::add_copy ()

Add a rule to buffer the original trace header. 








[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-rm_rule' /> public void exseis::piol::metadata::rules::Rule::rm_rule (Trace_metadata_key m)

Remove a rule based on the meta entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | m | The meta entry.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-extent' /> public size_t exseis::piol::metadata::rules::Rule::extent ()

Return the size of the buffer space required for the metadata items when converting to SEG-Y. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the size.  |












[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-extent-1' /> public size_t exseis::piol::metadata::rules::Rule::extent () const

Return the size of the buffer space required for the metadata items when converting to SEG-Y. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the size.  |







This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts. 




#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-memory_usage' /> public size_t exseis::piol::metadata::rules::Rule::memory_usage () const

Estimate of the total memory used. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return estimate in bytes.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-memory_usage_per_header' /> public size_t exseis::piol::metadata::rules::Rule::memory_usage_per_header () const

How much memory will each set of parameters require? 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Amount of memory in bytes.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-rules-Rule)

### <a name='exseis-piol-metadata-rules-Rule-get_entry' /> public const [Rule_entry][exseis-piol-metadata-rules-Rule_entry] * exseis::piol::metadata::rules::Rule::get_entry (Trace_metadata_key entry) const

Get the rule entry associated with a particular meta entry. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key] | entry | The meta entry. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| const [Rule_entry][exseis-piol-metadata-rules-Rule_entry] * | The associated rule entry, or nullptr if there is none.  |







If there is no rule entry associated with the meta entry, this function returns a nullptr.




#### Qualifiers: 
* const


[Go to Top](#exseis-piol-metadata-rules-Rule)

[exseis-piol-metadata-Trace_metadata_key]:./../index.md#exseis-piol-metadata-Trace_metadata_key
[exseis-piol-metadata-rules-Rule]:./Rule.md
[exseis-piol-metadata-rules-Rule-Rule_entry_map]:./Rule.md#exseis-piol-metadata-rules-Rule-Rule_entry_map
[exseis-piol-metadata-rules-Rule-State_flags]:./Rule/State_flags.md#exseis-piol-metadata-rules-Rule-State_flags
[exseis-piol-metadata-rules-Rule_entry]:./Rule_entry.md
[exseis-utils-Enum_hash]:./../../../utils/Enum_hash.md
