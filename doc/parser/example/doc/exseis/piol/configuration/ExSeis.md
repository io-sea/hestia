# <a name='exseis-piol-configuration-ExSeis' /> public exseis::piol::configuration::ExSeis

This class provides access to the [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] class but with a simpler API. 




## Inheritance:
Inherits from [exseis::piol::configuration::ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL].

## Public Static Functions
| Name | Description | 
| ---- | ---- |
| [make](#exseis-piol-configuration-ExSeis-make) | Constructor with optional maxLevel and which initialises MPI.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [~ExSeis](#exseis-piol-configuration-ExSeis-~ExSeis) | [ExSeis][exseis-piol-configuration-ExSeis] Deleter.  |
| [get_rank](#exseis-piol-configuration-ExSeis-get_rank) | Shortcut to get the commrank.  |
| [get_num_rank](#exseis-piol-configuration-ExSeis-get_num_rank) | Shortcut to get the number of ranks.  |
| [barrier](#exseis-piol-configuration-ExSeis-barrier) | Shortcut for a communication barrier.  |
| [max](#exseis-piol-configuration-ExSeis-max) | Return the maximum value amongst the processes.  |
| [assert_ok](#exseis-piol-configuration-ExSeis-assert_ok) | A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called and the code aborts.  |


## Private Functions
| Name | Description | 
| ---- | ---- |
| [ExSeis](#exseis-piol-configuration-ExSeis-ExSeis) | The constructor is private! Use the ExSeis::make(...) function.  |



## Public Static Functions
### <a name='exseis-piol-configuration-ExSeis-make' /> public std::shared_ptr< [ExSeis][exseis-piol-configuration-ExSeis] > exseis::piol::configuration::ExSeis::make (exseis::utils::Verbosity max_level=exseis::utils::Verbosity::none, MPI_Comm comm=MPI_COMM_WORLD)

Constructor with optional maxLevel and which initialises MPI. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | MPI_Comm | comm | The MPI communicator  |
| in | exseis::utils::Verbosity | max_level | The maximum log level to be recorded. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::shared_ptr< [ExSeis][exseis-piol-configuration-ExSeis] > | A shared pointer to a PIOL object.  |












#### Qualifiers: 
* static
* inline


[Go to Top](#exseis-piol-configuration-ExSeis)

## Public Functions
### <a name='exseis-piol-configuration-ExSeis-~ExSeis' /> public  exseis::piol::configuration::ExSeis::~ExSeis ()

[ExSeis][exseis-piol-configuration-ExSeis] Deleter. 








[Go to Top](#exseis-piol-configuration-ExSeis)

### <a name='exseis-piol-configuration-ExSeis-get_rank' /> public size_t exseis::piol::configuration::ExSeis::get_rank () const

Shortcut to get the commrank. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The comm rank.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeis)

### <a name='exseis-piol-configuration-ExSeis-get_num_rank' /> public size_t exseis::piol::configuration::ExSeis::get_num_rank () const

Shortcut to get the number of ranks. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The comm number of ranks.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeis)

### <a name='exseis-piol-configuration-ExSeis-barrier' /> public void exseis::piol::configuration::ExSeis::barrier () const

Shortcut for a communication barrier. 








#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeis)

### <a name='exseis-piol-configuration-ExSeis-max' /> public size_t exseis::piol::configuration::ExSeis::max (size_t n) const

Return the maximum value amongst the processes. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | n | The value to take part in the reduction |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the maximum value amongst the processes  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeis)

### <a name='exseis-piol-configuration-ExSeis-assert_ok' /> public void exseis::piol::configuration::ExSeis::assert_ok (const std::string &msg="") const

A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called and the code aborts. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::string & | msg | A message to be printed to the log. |








If an error has occured the log is printed, the object destructor is called and the code aborts. 




#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeis)

## Private Functions
### <a name='exseis-piol-configuration-ExSeis-ExSeis' /> private  exseis::piol::configuration::ExSeis::ExSeis (exseis::utils::Verbosity max_level=exseis::utils::Verbosity::none, MPI_Comm comm=MPI_COMM_WORLD)

The constructor is private! Use the ExSeis::make(...) function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | MPI_Comm | comm | The MPI communicator  |
| in | exseis::utils::Verbosity | max_level | The maximum log level to be recorded. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | A shared pointer to a PIOL object.  |












[Go to Top](#exseis-piol-configuration-ExSeis)

[exseis-piol-configuration-ExSeis]:./ExSeis.md
[exseis-piol-configuration-ExSeisPIOL]:./ExSeisPIOL.md
