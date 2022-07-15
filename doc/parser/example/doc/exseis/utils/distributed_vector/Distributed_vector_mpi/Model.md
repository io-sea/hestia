# <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model' /> protected exseis::utils::distributed_vector::Distributed_vector_mpi::Model

The internal implementation of [Distributed_vector_mpi][exseis-utils-distributed_vector-Distributed_vector_mpi]. 




## Inheritance:
Inherits from [exseis::utils::distributed_vector::Distributed_vector< T >::Concept][exseis-utils-distributed_vector-Distributed_vector-Concept].

## Public Attributes
| Name | Description | 
| ---- | ---- |
| [win](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-win) | The MPI window handle.  |
| [comm](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-comm) | The communicator the window is defined over.  |
| [data](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-data) | The local data, available for remote access.  |
| [global_size](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-global_size) | The global size of the array.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Model](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-Model) | Construct the global array.  |
| [set](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-set) | Set the global ith element with the given tuple.  |
| [get](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-get) | Get the global ith element.  |
| [size](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size) | Get the number of elements in the global array.  |



## Public Attributes
### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-win' /> public exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::win  = MPI_WIN_NULL

The MPI window handle. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-comm' /> public exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::comm  = MPI_COMM_NULL

The communicator the window is defined over. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-data' /> public exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::data  = nullptr

The local data, available for remote access. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-global_size' /> public exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::global_size  = 0

The global size of the array. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

## Public Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-Model' /> public  exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::Model (MPI_Aint global_size, MPI_Comm comm)

Construct the global array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | MPI_Aint | global_size | The number of elements in the global array.  |
| in | MPI_Comm | comm | The communicator to distribute the array over.  |







This operation is collective across all processes.

The global vector size must be set here, and will remain constant for the lifetime of the array. This is primarily because a pointer to the vector data is used in an MPI window, and resizing that array can invalidate the pointer.




[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-set' /> public void exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::set (size_t i, const T &val) override

Set the global ith element with the given tuple. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |
| in | const T & | val | The value to be set. |









> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size] 





> **post:** get(i) == val on all processes. (Explicit checking is subject to race conditions!) 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-get' /> public T exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::get (size_t i) const override

Get the global ith element. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | Return the value of the requested tuple. |












> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size] 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size' /> public size_t exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model::size () const override

Get the number of elements in the global array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the number of elements in the global array.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi-Model)

[exseis-utils-distributed_vector-Distributed_vector-Concept]:./../Distributed_vector/Concept.md
[exseis-utils-distributed_vector-Distributed_vector_mpi]:./../Distributed_vector_mpi.md
[exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size]:./Model.md#exseis-utils-distributed_vector-Distributed_vector_mpi-Model-size
