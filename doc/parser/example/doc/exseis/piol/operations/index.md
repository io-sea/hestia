# <a name='exseis-piol-operations' />  exseis::piol::operations

Basic operations performed on the file data. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Gather_info](./Gather_info.md) | A struct containing information about a seismic gather.  |
| [PIOL_CoordElem](./PIOL_CoordElem.md) | A structure to hold a reference to a single coordinate and the corresponding trace number. Defined as struct for C-compatibility.  |


## Namespaces
| Name | Description | 
| ---- | ---- |
| [sort_operations](./sort_operations/index.md) | Classes and functions for sorting the data.  |


## Type Definitions
| Name | Description | 
| ---- | ---- |
| [CoordElem](#exseis-piol-operations-CoordElem) | Import [PIOL_CoordElem][exseis-piol-operations-PIOL_CoordElem] from C API.  |
| [MinMaxFunc](#exseis-piol-operations-MinMaxFunc) | Return the value associated with a particular parameter.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [get_il_xl_gathers](#exseis-piol-operations-get_il_xl_gathers) | Find the inline/crossline for each il/xl gather and the number of traces per gather using the parameters from the file provided.  |
| [get_coord_min_max](#exseis-piol-operations-get_coord_min_max) | Get the min and max for a parameter. Use a second parameter to decide between equal cases.  |
| [get_min_max](#exseis-piol-operations-get_min_max) | Get the min and max for both parameters.  |
| [get_min_max](#exseis-piol-operations-get_min_max-1) | Get the min and the max of a set of parameters passed. This is a parallel operation. It is the collective min and max across all processes (which also must all call this file).  |



## Type Definitions
### <a name='exseis-piol-operations-CoordElem' /> public exseis::piol::operations::CoordElem 

Import [PIOL_CoordElem][exseis-piol-operations-PIOL_CoordElem] from C API. 










A structure to hold a reference to a single coordinate and the corresponding trace number. Defined as struct for C-compatibility.




[Go to Top](#exseis-piol-operations)

### <a name='exseis-piol-operations-MinMaxFunc' /> public exseis::piol::operations::MinMaxFunc 

Return the value associated with a particular parameter. 








[Go to Top](#exseis-piol-operations)

## Functions
### <a name='exseis-piol-operations-get_il_xl_gathers' /> public exseis::utils::Distributed_vector< [Gather_info][exseis-piol-operations-Gather_info] > exseis::piol::operations::get_il_xl_gathers (ExSeisPIOL *piol, Input_file *file)

Find the inline/crossline for each il/xl gather and the number of traces per gather using the parameters from the file provided. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | ExSeisPIOL * | piol | The piol object.  |
| in | Input_file * | file | The file which has il/xl gathers. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Distributed_vector< [Gather_info][exseis-piol-operations-Gather_info] > | Return an 'array' of tuples. Each tuple corresponds to each gather. Tuple elements: 1) Number of traces in the gather, 2) inline, 3) crossline.  |












[Go to Top](#exseis-piol-operations)

### <a name='exseis-piol-operations-get_coord_min_max' /> public std::vector< [CoordElem][exseis-piol-operations-CoordElem] > exseis::piol::operations::get_coord_min_max (ExSeisPIOL *piol, size_t offset, size_t sz, const T *coord, MinMaxFunc< T > elem1, MinMaxFunc< T > elem2)

Get the min and max for a parameter. Use a second parameter to decide between equal cases. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type of the input array |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | ExSeisPIOL * | piol | The PIOL object  |
| in | size_t | offset | The offset for the local process  |
| in | size_t | sz | The number of sets of parameters for the local process  |
| in | const T * | coord | The array of parameters for the local process  |
| in | [MinMaxFunc][exseis-piol-operations-MinMaxFunc]< T > | elem1 | The function for extracting the first parameter from `coord`  |
| in | [MinMaxFunc][exseis-piol-operations-MinMaxFunc]< T > | elem2 | The function for extracting the second parameter from `coord`  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< [CoordElem][exseis-piol-operations-CoordElem] > | Return a vector of length 2 with the Coordinates as elements  |












[Go to Top](#exseis-piol-operations)

### <a name='exseis-piol-operations-get_min_max' /> public void exseis::piol::operations::get_min_max (ExSeisPIOL *piol, size_t offset, size_t sz, const T *coord, MinMaxFunc< T > xlam, MinMaxFunc< T > ylam, CoordElem *minmax)

Get the min and max for both parameters. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type of the input array |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | ExSeisPIOL * | piol | The PIOL object  |
| in | size_t | offset | The offset for the local process  |
| in | size_t | sz | The number of sets of parameters for the local process  |
| in | const T * | coord | The array of parameters for the local process  |
| in | [MinMaxFunc][exseis-piol-operations-MinMaxFunc]< T > | xlam | The function for extracting the first parameter from `coord`  |
| in | [MinMaxFunc][exseis-piol-operations-MinMaxFunc]< T > | ylam | The function for extracting the second parameter from `coord`  |
| in | [CoordElem][exseis-piol-operations-CoordElem] * | minmax | An array of length 4 with the CoordElem structure as elements. min for xlam, max for xlam, min for ylam, max for ylam.  |












[Go to Top](#exseis-piol-operations)

### <a name='exseis-piol-operations-get_min_max-1' /> public void exseis::piol::operations::get_min_max (ExSeisPIOL *piol, size_t offset, size_t sz, Trace_metadata_key m1, Trace_metadata_key m2, const Trace_metadata &prm, CoordElem *minmax)

Get the min and the max of a set of parameters passed. This is a parallel operation. It is the collective min and max across all processes (which also must all call this file). 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | ExSeisPIOL * | piol | The PIOL object  |
| in | size_t | offset | The starting trace number (local).  |
| in | size_t | sz | The local number of traces to process.  |
| in | Trace_metadata_key | m1 | The first coordinate item of interest.  |
| in | Trace_metadata_key | m2 | The second coordinate item of interest.  |
| in | const Trace_metadata & | prm | An array of trace parameter structures  |
| out | [CoordElem][exseis-piol-operations-CoordElem] * | minmax | An array of structures containing the minimum item.x, maximum item.x, minimum item.y, maximum item.y and their respective trace numbers.  |












[Go to Top](#exseis-piol-operations)

[exseis-piol-operations-CoordElem]:./index.md#exseis-piol-operations-CoordElem
[exseis-piol-operations-Gather_info]:./Gather_info.md#exseis-piol-operations-Gather_info
[exseis-piol-operations-MinMaxFunc]:./index.md#exseis-piol-operations-MinMaxFunc
[exseis-piol-operations-PIOL_CoordElem]:./PIOL_CoordElem.md#exseis-piol-operations-PIOL_CoordElem
