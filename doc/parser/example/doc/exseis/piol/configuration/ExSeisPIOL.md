# <a name='exseis-piol-configuration-ExSeisPIOL' /> public exseis::piol::configuration::ExSeisPIOL

The [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] structure. A single instance of this structure should be created and passed to each subsequent PIOL object which is created. 




## Inheritance:
Is inherited by [exseis::piol::configuration::ExSeis][exseis-piol-configuration-ExSeis].

## Public Attributes
| Name | Description | 
| ---- | ---- |
| [log](#exseis-piol-configuration-ExSeisPIOL-log) | The [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] logger.  |
| [comm](#exseis-piol-configuration-ExSeisPIOL-comm) | The [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] communication.  |


## Protected Functions
| Name | Description | 
| ---- | ---- |
| [ExSeisPIOL](#exseis-piol-configuration-ExSeisPIOL-ExSeisPIOL) | Constructor which initialized the logging level and MPI.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [assert_ok](#exseis-piol-configuration-ExSeisPIOL-assert_ok) | A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called and the code aborts.  |



## Public Attributes
### <a name='exseis-piol-configuration-ExSeisPIOL-log' /> public exseis::piol::configuration::ExSeisPIOL::log 

The [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] logger. 








[Go to Top](#exseis-piol-configuration-ExSeisPIOL)

### <a name='exseis-piol-configuration-ExSeisPIOL-comm' /> public exseis::piol::configuration::ExSeisPIOL::comm 

The [ExSeisPIOL][exseis-piol-configuration-ExSeisPIOL] communication. 








[Go to Top](#exseis-piol-configuration-ExSeisPIOL)

## Protected Functions
### <a name='exseis-piol-configuration-ExSeisPIOL-ExSeisPIOL' /> protected  exseis::piol::configuration::ExSeisPIOL::ExSeisPIOL (exseis::utils::Verbosity max_level=exseis::utils::Verbosity::none, const Communicator_mpi::Opt &copt=Communicator_mpi::Opt())

Constructor which initialized the logging level and MPI. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Communicator_mpi::Opt][exseis-utils-communicator-Communicator_mpi-Opt] & | copt | An options structure for MPI  |
| in | exseis::utils::Verbosity | max_level | The maximum log level to be recorded.  |












[Go to Top](#exseis-piol-configuration-ExSeisPIOL)

## Public Functions
### <a name='exseis-piol-configuration-ExSeisPIOL-assert_ok' /> public void exseis::piol::configuration::ExSeisPIOL::assert_ok (const std::string &msg="") const

A function to check if an error has occured in the PIOL. If an error has occured the log is printed, the object destructor is called and the code aborts. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const std::string & | msg | A message to be printed to the log.  |












#### Qualifiers: 
* const


[Go to Top](#exseis-piol-configuration-ExSeisPIOL)

[exseis-piol-configuration-ExSeis]:./ExSeis.md
[exseis-piol-configuration-ExSeisPIOL]:./ExSeisPIOL.md
[exseis-utils-communicator-Communicator_mpi]:./../../utils/communicator/Communicator_mpi.md
[exseis-utils-communicator-Communicator_mpi-Opt]:./../../utils/communicator/Communicator_mpi/Opt.md#exseis-utils-communicator-Communicator_mpi-Opt
