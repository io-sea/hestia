# <a name='exseis-utils-communicator-Communicator_mpi' /> public exseis::utils::communicator::Communicator_mpi

The MPI communication class. All MPI communication specific routines should be wrapped up and accessible from this class. 




## Inheritance:
Inherits from [exseis::utils::communicator::Communicator][exseis-utils-communicator-Communicator].

## Classes
| Name | Description | 
| ---- | ---- |
| [Opt](./Communicator_mpi/Opt.md) | The MPI-Communicator options structure.  |


## Private Attributes
| Name | Description | 
| ---- | ---- |
| [m_comm](#exseis-utils-communicator-Communicator_mpi-m_comm) | The MPI communicator.  |
| [m_log](#exseis-utils-communicator-Communicator_mpi-m_log) | For logging messages.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Communicator_mpi](#exseis-utils-communicator-Communicator_mpi-Communicator_mpi) | The constructor.  |
| [get_comm](#exseis-utils-communicator-Communicator_mpi-get_comm) | Retrieve the MPI communicator associated with the ExSeisPIOL.  |
| [gather](#exseis-utils-communicator-Communicator_mpi-gather) | Pass a vector of double and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator_mpi-gather-1) | Pass a vector of double and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator_mpi-gather-2) | Pass a vector of exseis::utils::Integer and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator_mpi-gather-3) | Pass a vector of size_t and return the corresponding values to each process.  |
| [gather](#exseis-utils-communicator-Communicator_mpi-gather-4) | TMP.  |
| [sum](#exseis-utils-communicator-Communicator_mpi-sum) | Perform a reduce across all process to get the sum of the passed values.  |
| [max](#exseis-utils-communicator-Communicator_mpi-max) | Perform a reduce across all process to get the max of the passed values.  |
| [min](#exseis-utils-communicator-Communicator_mpi-min) | Perform a reduce across all process to get the min of the passed values.  |
| [offset](#exseis-utils-communicator-Communicator_mpi-offset) | Calculate the offset assuming a local given contribution.  |
| [barrier](#exseis-utils-communicator-Communicator_mpi-barrier) | A barrier between all processes which are members of the communication collective.  |



## Private Attributes
### <a name='exseis-utils-communicator-Communicator_mpi-m_comm' /> private exseis::utils::communicator::Communicator_mpi::m_comm 

The MPI communicator. 








[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-m_log' /> private exseis::utils::communicator::Communicator_mpi::m_log 

For logging messages. 








[Go to Top](#exseis-utils-communicator-Communicator_mpi)

## Public Functions
### <a name='exseis-utils-communicator-Communicator_mpi-Communicator_mpi' /> public  exseis::utils::communicator::Communicator_mpi::Communicator_mpi (exseis::utils::Log *log, const Communicator_mpi::Opt &opt)

The constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Log * | log | Pointer to log object  |
| in | const [Communicator_mpi::Opt][exseis-utils-communicator-Communicator_mpi-Opt] & | opt | Any options for the communication layer.  |












[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-get_comm' /> public MPI_Comm exseis::utils::communicator::Communicator_mpi::get_comm () const

Retrieve the MPI communicator associated with the ExSeisPIOL. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Comm | The MPI communicator.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-gather' /> public std::vector< float > exseis::utils::communicator::Communicator_mpi::gather (const std::vector< float > &in) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-gather-1' /> public std::vector< double > exseis::utils::communicator::Communicator_mpi::gather (const std::vector< double > &in) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-gather-2' /> public std::vector< exseis::utils::Integer > exseis::utils::communicator::Communicator_mpi::gather (const std::vector< exseis::utils::Integer > &in) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-gather-3' /> public std::vector< size_t > exseis::utils::communicator::Communicator_mpi::gather (const std::vector< size_t > &in) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-gather-4' /> public std::vector< T > exseis::utils::communicator::Communicator_mpi::gather (const T &val) const

TMP. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| class | T | TMP  |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const T & | val | TMP |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< T > | TMP |














> **[Todo][todo]:** DELETE ME 




#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-sum' /> public size_t exseis::utils::communicator::Communicator_mpi::sum (size_t val) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-max' /> public size_t exseis::utils::communicator::Communicator_mpi::max (size_t val) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-min' /> public size_t exseis::utils::communicator::Communicator_mpi::min (size_t val) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-offset' /> public size_t exseis::utils::communicator::Communicator_mpi::offset (size_t val) const override

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


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

### <a name='exseis-utils-communicator-Communicator_mpi-barrier' /> public void exseis::utils::communicator::Communicator_mpi::barrier () const override

A barrier between all processes which are members of the communication collective. 










Implementations of this pure virtual function will perform a collective wait. 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-communicator-Communicator_mpi)

[exseis-utils-communicator-Communicator]:./Communicator.md
[exseis-utils-communicator-Communicator_mpi-Opt]:./Communicator_mpi/Opt.md#exseis-utils-communicator-Communicator_mpi-Opt
[todo]:./../../../todo.md#todo
