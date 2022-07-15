# <a name='exseis-piol-operations-sort_operations' />  exseis::piol::operations::sort_operations

Classes and functions for sorting the data. 




## C API
| Name | Description | 
| ---- | ---- |
| [@1](#exseis-piol-operations-sort_operations-@1) | C API for [exseis::piol::operations::sort_operations::Sort_type][exseis-piol-operations-sort_operations-Sort_type].  |
| [exseis_SortType](#exseis-piol-operations-sort_operations-exseis_SortType) | C API for [exseis::piol::operations::sort_operations::Sort_type][exseis-piol-operations-sort_operations-Sort_type].  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [Sort_type](#exseis-piol-operations-sort_operations-Sort_type) | An enumeration of the different types of sorting operation.  |


## Type Definitions
| Name | Description | 
| ---- | ---- |
| [Compare](#exseis-piol-operations-sort_operations-Compare) | A template for the Compare less-than function.  |
| [CompareP](#exseis-piol-operations-sort_operations-CompareP) | A template for the Compare less-than function.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [get_sort_index](#exseis-piol-operations-sort_operations-get_sort_index) | Get the sorted index associated with a given list (support function)  |
| [sort](#exseis-piol-operations-sort_operations-sort) | Function to sort the metadata in a Trace_metadata struct. The returned vector is the location where the nth parameter is located in the sorted list.  |
| [sort](#exseis-piol-operations-sort_operations-sort-1) | Perform a sort on the given parameter structure.  |
| [check_order](#exseis-piol-operations-sort_operations-check_order) | Check that the file obeys the expected ordering.  |
| [get_comp](#exseis-piol-operations-sort_operations-get_comp) | Return the comparison function for the particular sort type.  |



## C API
### <a name='exseis-piol-operations-sort_operations-@1' /> public exseis::piol::operations::sort_operations::@1

C API for [exseis::piol::operations::sort_operations::Sort_type][exseis-piol-operations-sort_operations-Sort_type]. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| exseis_sorttype_SrcRcv | Sort by source x, source y, receiver x, receiver y.  |  |
| exseis_sorttype_SrcOff | Sort by source x, source y, calcuated offset.  |  |
| exseis_sorttype_SrcROff | Sort by source x, source y, offset read from file.  |  |
| exseis_sorttype_RcvOff | Sort by receiver x, receiver y, calculate offset.  |  |
| exseis_sorttype_RcvROff | Sort by receiver x, receiver y, offset read from file.  |  |
| exseis_sorttype_LineOff | Sort by inline, crossline, calculated offset.  |  |
| exseis_sorttype_LineROff | Sort by inline, crossline, offset read from file.  |  |
| exseis_sorttype_OffLine | Sort by calculated offset, inline, crossline.  |  |
| exseis_sorttype_ROffLine | Sort by offset read from file, inline, crossline.  |  |





An enumeration of the different types of sorting operation.




[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-exseis_SortType' /> public exseis::piol::operations::sort_operations::exseis_SortType 

C API for [exseis::piol::operations::sort_operations::Sort_type][exseis-piol-operations-sort_operations-Sort_type]. 










An enumeration of the different types of sorting operation.




[Go to Top](#exseis-piol-operations-sort_operations)

## Enumerations
### <a name='exseis-piol-operations-sort_operations-Sort_type' /> public exseis::piol::operations::sort_operations::Sort_type

An enumeration of the different types of sorting operation. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| SrcRcv | Sort by source x, source y, receiver x, receiver y.  |  |
| SrcOff | Sort by source x, source y, calcuated offset.  |  |
| SrcROff | Sort by source x, source y, offset read from file.  |  |
| RcvOff | Sort by receiver x, receiver y, calculate offset.  |  |
| RcvROff | Sort by receiver x, receiver y, offset read from file.  |  |
| LineOff | Sort by inline, crossline, calculated offset.  |  |
| LineROff | Sort by inline, crossline, offset read from file.  |  |
| OffLine | Sort by calculated offset, inline, crossline.  |  |
| ROffLine | Sort by offset read from file, inline, crossline.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-piol-operations-sort_operations)

## Type Definitions
### <a name='exseis-piol-operations-sort_operations-Compare' /> public exseis::piol::operations::sort_operations::Compare 

A template for the Compare less-than function. 








[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-CompareP' /> public exseis::piol::operations::sort_operations::CompareP 

A template for the Compare less-than function. 








[Go to Top](#exseis-piol-operations-sort_operations)

## Functions
### <a name='exseis-piol-operations-sort_operations-get_sort_index' /> public std::vector< size_t > exseis::piol::operations::sort_operations::get_sort_index (size_t sz, const size_t *list)

Get the sorted index associated with a given list (support function) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The length of the list  |
| in | const size_t * | list | The array of numbers |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< size_t > | A vector containing the numbering of list in a sorted order  |












[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-sort' /> public std::vector< size_t > exseis::piol::operations::sort_operations::sort (ExSeisPIOL *piol, Trace_metadata &prm, CompareP comp, bool file_order=true)

Function to sort the metadata in a Trace_metadata struct. The returned vector is the location where the nth parameter is located in the sorted list. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | ExSeisPIOL * | piol | The PIOL object.  |
| inout | Trace_metadata & | prm | The parameter structure to sort  |
| in | [CompareP][exseis-piol-operations-sort_operations-CompareP] | comp | The Trace_metadata function to use for less-than comparisons between objects in the vector. It assumes each Trace_metadata structure has exactly one entry.  |
| in | bool | file_order | Do we wish to have the sort in the sorted input order (true) or sorted order (false) |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< size_t > | Return the correct order of traces from those which are smallest with respect to the comp function.  |







Implementation note: the Trace_metadata vector is used internally to allow random-access iterator support.




[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-sort-1' /> public std::vector< size_t > exseis::piol::operations::sort_operations::sort (ExSeisPIOL *piol, Sort_type type, Trace_metadata &prm)

Perform a sort on the given parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | ExSeisPIOL * | piol | The PIOL object  |
| in | [Sort_type][exseis-piol-operations-sort_operations-Sort_type] | type | The sort type  |
| inout | Trace_metadata & | prm | The trace parameter structure. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< size_t > | Return a vector which is a list of the ordered trace numbers. i.e the 0th member is the position of the 0th trace post-sort.  |












[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-check_order' /> public bool exseis::piol::operations::sort_operations::check_order (const Input_file &src, exseis::utils::Contiguous_decomposition dec, Sort_type type)

Check that the file obeys the expected ordering. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const Input_file & | src | The input file.  |
| in | exseis::utils::Contiguous_decomposition | dec | The decomposition: a pair which contains the `offset` and the number of traces (`size`) for the local process.  |
| in | [Sort_type][exseis-piol-operations-sort_operations-Sort_type] | type | The sort type |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Return true if the local ordering is correct.  |












[Go to Top](#exseis-piol-operations-sort_operations)

### <a name='exseis-piol-operations-sort_operations-get_comp' /> public [CompareP][exseis-piol-operations-sort_operations-CompareP] exseis::piol::operations::sort_operations::get_comp (Sort_type type)

Return the comparison function for the particular sort type. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Sort_type][exseis-piol-operations-sort_operations-Sort_type] | type | The sort type |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [CompareP][exseis-piol-operations-sort_operations-CompareP] | A std::function object with the correct comparison for the sort type.  |












[Go to Top](#exseis-piol-operations-sort_operations)

[exseis-piol-operations-sort_operations-CompareP]:./index.md#exseis-piol-operations-sort_operations-CompareP
[exseis-piol-operations-sort_operations-Sort_type]:./index.md#exseis-piol-operations-sort_operations-Sort_type
