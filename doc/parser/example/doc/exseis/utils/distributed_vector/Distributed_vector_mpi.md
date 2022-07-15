# <a name='exseis-utils-distributed_vector-Distributed_vector_mpi' /> public exseis::utils::distributed_vector::Distributed_vector_mpi

An MPI implementation of [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector]. 




## Inheritance:
Inherits from [exseis::utils::distributed_vector::Distributed_vector< T >][exseis-utils-distributed_vector-Distributed_vector].

## Classes
| Name | Description | 
| ---- | ---- |
| [Model](./Distributed_vector_mpi/Model.md) | The internal implementation of [Distributed_vector_mpi][exseis-utils-distributed_vector-Distributed_vector_mpi].  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Distributed_vector_mpi](#exseis-utils-distributed_vector-Distributed_vector_mpi-Distributed_vector_mpi) | Construct the global array.  |



## Public Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector_mpi-Distributed_vector_mpi' /> public  exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Distributed_vector_mpi (size_t global_size, MPI_Comm comm)

Construct the global array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | global_size | The number of elements in the global array.  |
| in | MPI_Comm | comm | The communicator to distribute the array over.  |







This operation is collective across all processes.

The global vector size must be set here, and will remain constant for the lifetime of the array. This is primarily because a pointer to the vector data is used in an MPI window, and resizing that array can invalidate the pointer.




[Go to Top](#exseis-utils-distributed_vector-Distributed_vector_mpi)

[exseis-utils-distributed_vector-Distributed_vector]:./Distributed_vector.md
[exseis-utils-distributed_vector-Distributed_vector-Concept]:./Distributed_vector/Concept.md
[exseis-utils-distributed_vector-Distributed_vector_mpi]:./Distributed_vector_mpi.md
