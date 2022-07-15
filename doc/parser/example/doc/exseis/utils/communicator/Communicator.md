# <a name='exseis-utils-communicator-Communicator' /> public exseis::utils::communicator::Communicator

The Communication layer interface. Specific communication implementations work off this base class. 




## Inheritance:
Is inherited by [exseis::utils::communicator::Communicator_mpi][exseis-utils-communicator-Communicator_mpi].

## Protected Attributes
| Name | Description | 
| ---- | ---- |
| [m_rank](#exseis-utils-communicator-Communicator-m_rank) | A number in the sequence from 0 to some maximum (num_rank-1) which indicates the process number.  |
| [m_num_rank](#exseis-utils-communicator-Communicator-m_num_rank) | The total number of processes which are executing together.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [~Communicator](#exseis-utils-communicator-Communicator-~Communicator) | A virtual destructor to allow deletion.  |
| [get_rank](#exseis-utils-communicator-Communicator-get_rank) | Returns the rank of the process executing the function/.  |
| [get_num_rank](#exseis-utils-communicator-Communicator-get_num_rank) | Returns the number of processes which are executing together.  |
| [gather](#exseis-utils-communicator-Communicator-gather) | Pass a vector of double and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator-gather-1) | Pass a vector of double and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator-gather-2) | Pass a vector of exseis::utils::Integer and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator-gather-3) | Pass a vector of size_t and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator-gather-4) | Pass a value and return the corresponding values to each process.  |
| [sum](#exseis-utils-communicator-Communicator-sum) | Perform a reduce across all process to get the sum of the passed values.  |
| [max](#exseis-utils-communicator-Communicator-max) | Perform a reduce across all process to get the max of the passed values.  |
| [min](#exseis-utils-communicator-Communicator-min) | Perform a reduce across all process to get the min of the passed values.  |
| [offset](#exseis-utils-communicator-Communicator-offset) | Calculate the offset assuming a local given contribution.  |
| [barrier](#exseis-utils-communicator-Communicator-barrier) | A barrier between all processes which are members of the communication collective.  |



## Protected Attributes
### <a name='exseis-utils-communicator-Communicator-m_rank' /> protected exseis::utils::communicator::Communicator::m_rank 

A number in the sequence from 0 to some maximum (num_rank-1) which indicates the process number. 








[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-m_num_rank' /> protected exseis::utils::communicator::Communicator::m_num_rank 

The total number of processes which are executing together. 








[Go to Top](#exseis-utils-communicator-Communicator)

## Public Functions
### <a name='exseis-utils-communicator-Communicator-~Communicator' /> public  exseis::utils::communicator::Communicator::~Communicator ()=default

A virtual destructor to allow deletion. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-get_rank' /> public size_t exseis::utils::communicator::Communicator::get_rank () const

Returns the rank of the process executing the function/. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The rank.  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-get_num_rank' /> public size_t exseis::utils::communicator::Communicator::get_num_rank () const

Returns the number of processes which are executing together. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of processes (i.e number of ranks).  |












#### Qualifiers: 
* const
* inline
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-gather' /> public std::vector< double > exseis::utils::communicator::Communicator::gather (const std::vector< double > &val) const =0

Pass a vector of double and return the corresponding values to each process. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::vector< double > & | val | The local value to use in the gather |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< double > | Return a vector where the nth element is the value from the nth rank.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-gather-1' /> public std::vector< float > exseis::utils::communicator::Communicator::gather (const std::vector< float > &val) const =0

Pass a vector of double and return the corresponding values to each process. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::vector< float > & | val | The local value to use in the gather |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< float > | Return a vector where the nth element is the value from the nth rank.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-gather-2' /> public std::vector< exseis::utils::Integer > exseis::utils::communicator::Communicator::gather (const std::vector< exseis::utils::Integer > &val) const =0

Pass a vector of exseis::utils::Integer and return the corresponding values to each process. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::vector< exseis::utils::Integer > & | val | The local value to use in the gather |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< exseis::utils::Integer > | Return a vector where the nth element is the value from the nth rank.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-gather-3' /> public std::vector< size_t > exseis::utils::communicator::Communicator::gather (const std::vector< size_t > &val) const =0

Pass a vector of size_t and return the corresponding values to each process. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::vector< size_t > & | val | The local value to use in the gather |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< size_t > | Return a vector where the nth element is the value from the nth rank.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-gather-4' /> public std::vector< T > exseis::utils::communicator::Communicator::gather (const T &val) const

Pass a value and return the corresponding values to each process. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| class | T | The type use for the gather. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const T & | val | The local value to use in the gather |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< T > | Return a vector where the nth element is the value from the nth rank.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-sum' /> public size_t exseis::utils::communicator::Communicator::sum (size_t val) const =0

Perform a reduce across all process to get the sum of the passed values. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | val | variable to be used in the operation from this process |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the global sum (same value on all processes)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-max' /> public size_t exseis::utils::communicator::Communicator::max (size_t val) const =0

Perform a reduce across all process to get the max of the passed values. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | val | variable to be used in the operation from this process |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the global max (same value on all processes)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-min' /> public size_t exseis::utils::communicator::Communicator::min (size_t val) const =0

Perform a reduce across all process to get the min of the passed values. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | val | variable to be used in the operation from this process |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the global min (same value on all processes)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-offset' /> public size_t exseis::utils::communicator::Communicator::offset (size_t val) const =0

Calculate the offset assuming a local given contribution. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | val | variable to be used in the operation from this process |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | the local offset (equivalent to an MPI exscan)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

### <a name='exseis-utils-communicator-Communicator-barrier' /> public void exseis::utils::communicator::Communicator::barrier () const =0

A barrier between all processes which are members of the communication collective. 










Implementations of this pure virtual function will perform a collective wait. 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator)

[exseis-utils-communicator-Communicator_mpi]:./Communicator_mpi.md
