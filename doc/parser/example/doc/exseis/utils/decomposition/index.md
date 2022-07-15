# <a name='exseis-utils-decomposition' />  exseis::utils::decomposition

Functions for splitting ranges across processes. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Contiguous_decomposition](./Contiguous_decomposition.md) | The [`Contiguous_decomposition`][exseis-utils-decomposition-Contiguous_decomposition] class represents a section of a range on the given rank which has been distributed over a number of ranks.  |
| [Decomposition_index_location](./Decomposition_index_location.md) | This struct represents the location and local index of a global index in a decomposed range.  |
| [exseis_Contiguous_decomposition](./exseis_Contiguous_decomposition.md) | The [`Contiguous_decomposition`][exseis-utils-decomposition-Contiguous_decomposition] class represents a section of a range on the given rank which has been distributed over a number of ranks.  |


## C API
| Name | Description | 
| ---- | ---- |
| [exseis_block_decomposition](#exseis-utils-decomposition-exseis_block_decomposition) | Perform a 1d decomposition of the interval [0,range_size-1] into `num_ranks` pieces so it is optimally spread across each `rank`.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [block_decomposition](#exseis-utils-decomposition-block_decomposition) | Perform a 1d decomposition of the interval [0,range_size-1] into `num_ranks` pieces so it is optimally spread across each `rank`.  |
| [block_decomposition_location](#exseis-utils-decomposition-block_decomposition_location) | Find the rank and local index for a global index of a block decomposed range.  |



## C API
### <a name='exseis-utils-decomposition-exseis_block_decomposition' /> public struct [exseis_Contiguous_decomposition][exseis-utils-decomposition-exseis_Contiguous_decomposition] exseis::utils::decomposition::exseis_block_decomposition (size_t range_size, size_t num_ranks, size_t rank)

Perform a 1d decomposition of the interval [0,range_size-1] into `num_ranks` pieces so it is optimally spread across each `rank`. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | range_size | The total size of the 1d domain.  |
| in | size_t | num_ranks | The number of ranks to perform the decomposition over.  |
| in | size_t | rank | The rank of the local process. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| struct [exseis_Contiguous_decomposition][exseis-utils-decomposition-exseis_Contiguous_decomposition] | The section of the range decomposed onto rank `rank`. |







C API for [block_decomposition()][exseis-utils-decomposition-block_decomposition] 





> **pre:** num_ranks > 0 





> **pre:** rank < num_ranks 




[Go to Top](#exseis-utils-decomposition)

## Functions
### <a name='exseis-utils-decomposition-block_decomposition' /> public [Contiguous_decomposition][exseis-utils-decomposition-Contiguous_decomposition] exseis::utils::decomposition::block_decomposition (size_t range_size, size_t num_ranks, size_t rank)

Perform a 1d decomposition of the interval [0,range_size-1] into `num_ranks` pieces so it is optimally spread across each `rank`. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | range_size | The total size of the 1d domain.  |
| in | size_t | num_ranks | The number of ranks to perform the decomposition over.  |
| in | size_t | rank | The rank of the local process. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Contiguous_decomposition][exseis-utils-decomposition-Contiguous_decomposition] | The section of the range decomposed onto rank `rank`. |












> **pre:** num_ranks > 0 





> **pre:** rank < num_ranks 




[Go to Top](#exseis-utils-decomposition)

### <a name='exseis-utils-decomposition-block_decomposition_location' /> public [Decomposition_index_location][exseis-utils-decomposition-Decomposition_index_location] exseis::utils::decomposition::block_decomposition_location (size_t range_size, size_t num_ranks, size_t global_index)

Find the rank and local index for a global index of a block decomposed range. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | range_size | The size of the decomposed range.  |
| in | size_t | num_ranks | The number of ranks the range was decomposed over.  |
| in | size_t | global_index | The requested index in the range. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Decomposition_index_location][exseis-utils-decomposition-Decomposition_index_location] | A Decomposed_index_location containing the `rank` the global_index was decomposed to, and the `local_index` representing the distance between the local range offset and the global index. |












> **pre:** num_ranks > 0 





> **pre:** global_index < range_size





> **post:** return.rank < num_ranks 





> **post:** return.local_index <= global_index 




[Go to Top](#exseis-utils-decomposition)

[exseis-utils-decomposition-Contiguous_decomposition]:./Contiguous_decomposition.md#exseis-utils-decomposition-Contiguous_decomposition
[exseis-utils-decomposition-Decomposition_index_location]:./Decomposition_index_location.md#exseis-utils-decomposition-Decomposition_index_location
[exseis-utils-decomposition-block_decomposition]:./index.md#exseis-utils-decomposition-block_decomposition
[exseis-utils-decomposition-exseis_Contiguous_decomposition]:./exseis_Contiguous_decomposition.md#exseis-utils-decomposition-exseis_Contiguous_decomposition
