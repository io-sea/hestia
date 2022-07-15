# <a name='exseis-utils-distributed_vector-Distributed_vector-Concept' /> protected exseis::utils::distributed_vector::Distributed_vector::Concept

An abstract interface for implementing the [`Distributed_vector`][exseis-utils-distributed_vector-Distributed_vector]. 



This class is used to implement the Type Erasure used in [`Distributed_vector`][exseis-utils-distributed_vector-Distributed_vector].

Classes deriving from [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector] should derive from this class and put their actual implementation in here. A specialized instance of this `
[Concept][exseis-utils-distributed_vector-Distributed_vector-Concept]` class should then be passed to the `
[Distributed_vector][exseis-utils-distributed_vector-Distributed_vector]` constructor during construction of a child class. The `
[Distributed_vector][exseis-utils-distributed_vector-Distributed_vector]` class will then call the virtual member functions of this class.

This will be held in a std::unique_ptr in the [`Distributed_vector`][exseis-utils-distributed_vector-Distributed_vector] instance. This allows for specialization of the `
[Distributed_vector][exseis-utils-distributed_vector-Distributed_vector]` class, along with value semantics while passing it around. 




## Inheritance:
Is inherited by [exseis::utils::distributed_vector::Distributed_vector_mpi< T >::Model][exseis-utils-distributed_vector-Distributed_vector_mpi-Model].

## Public Functions
| Name | Description | 
| ---- | ---- |
| [~Concept](#exseis-utils-distributed_vector-Distributed_vector-Concept-~Concept) | Virtual destructor.  |
| [set](#exseis-utils-distributed_vector-Distributed_vector-Concept-set) | Set the global ith element with the given tuple.  |
| [get](#exseis-utils-distributed_vector-Distributed_vector-Concept-get) | Get the global ith element.  |
| [size](#exseis-utils-distributed_vector-Distributed_vector-Concept-size) | Get the number of elements in the global array.  |



## Public Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector-Concept-~Concept' /> public  exseis::utils::distributed_vector::Distributed_vector< T >::Concept::~Concept ()=default

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector-Concept)

### <a name='exseis-utils-distributed_vector-Distributed_vector-Concept-set' /> public void exseis::utils::distributed_vector::Distributed_vector< T >::Concept::set (size_t i, const T &val)=0

Set the global ith element with the given tuple. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |
| in | const T & | val | The value to be set. |









> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector-Concept-size] 





> **post:** get(i) == val on all processes. (Explicit checking is subject to race conditions!) 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector-Concept)

### <a name='exseis-utils-distributed_vector-Distributed_vector-Concept-get' /> public T exseis::utils::distributed_vector::Distributed_vector< T >::Concept::get (size_t i) const =0

Get the global ith element. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | Return the value of the requested tuple. |












> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector-Concept-size] 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector-Concept)

### <a name='exseis-utils-distributed_vector-Distributed_vector-Concept-size' /> public size_t exseis::utils::distributed_vector::Distributed_vector< T >::Concept::size () const =0

Get the number of elements in the global array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the number of elements in the global array.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector-Concept)

[exseis-utils-distributed_vector-Distributed_vector]:./../Distributed_vector.md
[exseis-utils-distributed_vector-Distributed_vector-Concept]:./Concept.md
[exseis-utils-distributed_vector-Distributed_vector-Concept-size]:./Concept.md#exseis-utils-distributed_vector-Distributed_vector-Concept-size
[exseis-utils-distributed_vector-Distributed_vector_mpi-Model]:./../Distributed_vector_mpi/Model.md
