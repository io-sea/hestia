# <a name='exseis-piol-io_driver-IO_driver_mpi-Opt' /> public exseis::piol::io_driver::IO_driver_mpi::Opt

The MPI-IO options structure. 



This class also manages the lifetime of an MPI_Info object. 




## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [Opt](#exseis-piol-io_driver-IO_driver_mpi-Opt-Opt) | Construct [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and create an instance of MPI_Info.  |
| [free](#exseis-piol-io_driver-IO_driver_mpi-Opt-free) | Delete [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and free the instance of MPI_Info.  |
| [~Opt](#exseis-piol-io_driver-IO_driver_mpi-Opt-~Opt) | Delete [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and free the instance of MPI_Info.  |
| [Opt](#exseis-piol-io_driver-IO_driver_mpi-Opt-Opt-1) | Copy constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver_mpi-Opt-operator=) | Copy assignment: delete.  |
| [Opt](#exseis-piol-io_driver-IO_driver_mpi-Opt-Opt-2) | Move constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver_mpi-Opt-operator=-1) | Move assignment: delete.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [Type](#exseis-piol-io_driver-IO_driver_mpi-Opt-Type) | The Type of the class this structure is nested in.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [use_collective_operations](#exseis-piol-io_driver-IO_driver_mpi-Opt-use_collective_operations) | Whether collective read/write operations will be used.  |
| [info](#exseis-piol-io_driver-IO_driver_mpi-Opt-info) | The info structure to use.  |
| [max_size](#exseis-piol-io_driver-IO_driver_mpi-Opt-max_size) | The maximum size to allow to be written to disk per process in one operation.  |
| [file_communicator](#exseis-piol-io_driver-IO_driver_mpi-Opt-file_communicator) | The MPI communicator to use for file access.  |



## Special Member Functions
### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-Opt' /> public  exseis::piol::io_driver::IO_driver_mpi::Opt::Opt ()

Construct [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and create an instance of MPI_Info. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-free' /> public void exseis::piol::io_driver::IO_driver_mpi::Opt::free ()

Delete [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and free the instance of MPI_Info. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-~Opt' /> public  exseis::piol::io_driver::IO_driver_mpi::Opt::~Opt ()

Delete [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt], and free the instance of MPI_Info. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-Opt-1' /> public  exseis::piol::io_driver::IO_driver_mpi::Opt::Opt (const Opt &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-operator=' /> public [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & exseis::piol::io_driver::IO_driver_mpi::Opt::operator= (const Opt &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-Opt-2' /> public  exseis::piol::io_driver::IO_driver_mpi::Opt::Opt (Opt &&)=delete

Move constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-operator=-1' /> public [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & exseis::piol::io_driver::IO_driver_mpi::Opt::operator= (Opt &&)=delete

Move assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

## Public Types
### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-Type' /> public exseis::piol::io_driver::IO_driver_mpi::Opt::Type 

The Type of the class this structure is nested in. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

## Public Attributes
### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-use_collective_operations' /> public exseis::piol::io_driver::IO_driver_mpi::Opt::use_collective_operations 

Whether collective read/write operations will be used. 










Default value dependant on EXSEISDAT_MPIIO_COLLECTIVES definition. 




[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-info' /> public exseis::piol::io_driver::IO_driver_mpi::Opt::info 

The info structure to use. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-max_size' /> public exseis::piol::io_driver::IO_driver_mpi::Opt::max_size  = exseis::utils::mpi_max_array_length<int32_t>()

The maximum size to allow to be written to disk per process in one operation. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

### <a name='exseis-piol-io_driver-IO_driver_mpi-Opt-file_communicator' /> public exseis::piol::io_driver::IO_driver_mpi::Opt::file_communicator  = MPI_COMM_WORLD

The MPI communicator to use for file access. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi-Opt)

[exseis-piol-io_driver-IO_driver_mpi]:./../IO_driver_mpi.md
[exseis-piol-io_driver-IO_driver_mpi-Opt]:./Opt.md#exseis-piol-io_driver-IO_driver_mpi-Opt
