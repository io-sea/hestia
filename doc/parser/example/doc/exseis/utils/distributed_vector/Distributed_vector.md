# <a name='exseis-utils-distributed_vector-Distributed_vector' /> public exseis::utils::distributed_vector::Distributed_vector

A structure for global arrays. 



This classes uses a sort of Type Erasure to hide the implementation details of deriving classes. This allows for both inheritance of an abstract interface and value semantics.



## Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type of variable to store in the vector.  |


## Inheritance:
Is inherited by [exseis::utils::distributed_vector::Distributed_vector_mpi< T >][exseis-utils-distributed_vector-Distributed_vector_mpi].

## Classes
| Name | Description | 
| ---- | ---- |
| [Concept](./Distributed_vector/Concept.md) | An abstract interface for implementing the [`Distributed_vector`][exseis-utils-distributed_vector-Distributed_vector].  |


## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [Distributed_vector](#exseis-utils-distributed_vector-Distributed_vector-Distributed_vector) | Default constructor: delete.  |
| [~Distributed_vector](#exseis-utils-distributed_vector-Distributed_vector-~Distributed_vector) | Default destructor.  |
| [Distributed_vector](#exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-1) | Move constructor: default.  |
| [operator=](#exseis-utils-distributed_vector-Distributed_vector-operator=) | Move assignment: default.  |
| [Distributed_vector](#exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-2) | Copy constructor: delete.  |
| [operator=](#exseis-utils-distributed_vector-Distributed_vector-operator=-1) | Copy assignment: delete.  |


## Protected Attributes
| Name | Description | 
| ---- | ---- |
| [m_concept](#exseis-utils-distributed_vector-Distributed_vector-m_concept) | The instance of the private implementation for [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector].  |


## Protected Functions
| Name | Description | 
| ---- | ---- |
| [Distributed_vector](#exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-3) | Constructor passes instance of [Concept][exseis-utils-distributed_vector-Distributed_vector-Concept] to the `m_concept` member variable.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [set](#exseis-utils-distributed_vector-Distributed_vector-set) | Set the global ith element with the given tuple.  |
| [get](#exseis-utils-distributed_vector-Distributed_vector-get) | Get the global ith element.  |
| [operator[]](#exseis-utils-distributed_vector-Distributed_vector-operator[]) | Get the global ith element.  |
| [size](#exseis-utils-distributed_vector-Distributed_vector-size) | Get the number of elements in the global array.  |



## Special Member Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector-Distributed_vector' /> public  exseis::utils::distributed_vector::Distributed_vector< T >::Distributed_vector ()=delete

Default constructor: delete. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-~Distributed_vector' /> public  exseis::utils::distributed_vector::Distributed_vector< T >::~Distributed_vector ()=default

Default destructor. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-1' /> public  exseis::utils::distributed_vector::Distributed_vector< T >::Distributed_vector (Distributed_vector &&)=default

Move constructor: default. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-operator=' /> public [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector] & exseis::utils::distributed_vector::Distributed_vector< T >::operator= (Distributed_vector &&)=default

Move assignment: default. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector] & | A reference to the current instance.  |












[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-2' /> public  exseis::utils::distributed_vector::Distributed_vector< T >::Distributed_vector (const Distributed_vector &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-operator=-1' /> public [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector] & exseis::utils::distributed_vector::Distributed_vector< T >::operator= (const Distributed_vector &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector] & | A reference to the current instance.  |












[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

## Protected Attributes
### <a name='exseis-utils-distributed_vector-Distributed_vector-m_concept' /> protected exseis::utils::distributed_vector::Distributed_vector< T >::m_concept 

The instance of the private implementation for [Distributed_vector][exseis-utils-distributed_vector-Distributed_vector]. 








[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

## Protected Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector-Distributed_vector-3' /> protected  exseis::utils::distributed_vector::Distributed_vector< T >::Distributed_vector (std::unique_ptr< Concept > concept)

Constructor passes instance of [Concept][exseis-utils-distributed_vector-Distributed_vector-Concept] to the `m_concept` member variable. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::unique_ptr< [Concept][exseis-utils-distributed_vector-Distributed_vector-Concept] > | concept | The inherited and specialized [Concept][exseis-utils-distributed_vector-Distributed_vector-Concept] class defined by a child class.  |







This is intended to be the point of entry for inheriting classes.




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

## Public Functions
### <a name='exseis-utils-distributed_vector-Distributed_vector-set' /> public void exseis::utils::distributed_vector::Distributed_vector< T >::set (size_t i, const T &val)

Set the global ith element with the given tuple. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |
| in | const T & | val | The value to be set. |









> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector-size] 





> **post:** get(i) == val on all processes. (Explicit checking is subject to race conditions!) 




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-get' /> public T exseis::utils::distributed_vector::Distributed_vector< T >::get (size_t i) const

Get the global ith element. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | Return the value of the requested tuple. |












> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector-size] 




#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-operator[]' /> public T exseis::utils::distributed_vector::Distributed_vector< T >::operator[] (size_t i) const

Get the global ith element. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The index into the global array.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | Return the value of the requested tuple. |












> **pre:** i < [size()][exseis-utils-distributed_vector-Distributed_vector-size] 




#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

### <a name='exseis-utils-distributed_vector-Distributed_vector-size' /> public size_t exseis::utils::distributed_vector::Distributed_vector< T >::size () const

Get the number of elements in the global array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the number of elements in the global array.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-distributed_vector-Distributed_vector)

[exseis-utils-distributed_vector-Distributed_vector]:./Distributed_vector.md
[exseis-utils-distributed_vector-Distributed_vector-Concept]:./Distributed_vector/Concept.md
[exseis-utils-distributed_vector-Distributed_vector-size]:./Distributed_vector.md#exseis-utils-distributed_vector-Distributed_vector-size
[exseis-utils-distributed_vector-Distributed_vector_mpi]:./Distributed_vector_mpi.md
[exseis-utils-distributed_vector-Distributed_vector_mpi-Model]:./Distributed_vector_mpi/Model.md
