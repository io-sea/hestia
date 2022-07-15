# <a name='exseis-utils-mpi_utils' />  exseis::utils::mpi_utils

MPI utilities. 




## Functions
| Name | Description | 
| ---- | ---- |
| [mpi_error_to_string](#exseis-utils-mpi_utils-mpi_error_to_string) | Generate a human readable MPI error message from an MPI error code and status object.  |
| [mpi_max_array_length](#exseis-utils-mpi_utils-mpi_max_array_length) | Return the known limit for the array length of a given type for the MPI implementation.  |
| [mpi_max_array_length](#exseis-utils-mpi_utils-mpi_max_array_length-1) | Return the known limit for the array length of a given type for the MPI implementation.  |



## Functions
### <a name='exseis-utils-mpi_utils-mpi_error_to_string' /> public std::string exseis::utils::mpi_utils::mpi_error_to_string (int mpi_error, const MPI_Status *mpi_status=MPI_STATUS_IGNORE)

Generate a human readable MPI error message from an MPI error code and status object. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int | mpi_error | The MPI error code  |
| in | const MPI_Status * | mpi_status | The MPI status object (optional). |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::string | A human readable string detailing the error code and the status.  |












[Go to Top](#exseis-utils-mpi_utils)

### <a name='exseis-utils-mpi_utils-mpi_max_array_length' /> public size_t exseis::utils::mpi_utils::mpi_max_array_length (size_t type_size)

Return the known limit for the array length of a given type for the MPI implementation. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | type_size | The size of the type to check the limit for. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The maximum number of items in the array.  |












[Go to Top](#exseis-utils-mpi_utils)

### <a name='exseis-utils-mpi_utils-mpi_max_array_length-1' /> public size_t exseis::utils::mpi_utils::mpi_max_array_length ()

Return the known limit for the array length of a given type for the MPI implementation. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type to find the array length limit for. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The maximum number of items in the array.  |












[Go to Top](#exseis-utils-mpi_utils)

