# <a name='exseis-piol-io_driver' />  exseis::piol::io_driver

Interface and implementations that perform I/O operations. 




## Classes
| Name | Description | 
| ---- | ---- |
| [IO_driver](./IO_driver.md) | The Data layer interface. Specific data I/O implementations work off this base class.  |
| [IO_driver_mpi](./IO_driver_mpi.md) | The MPI-IO Data class.  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [FileMode](#exseis-piol-io_driver-FileMode) | The file modes possible for files.  |



## Enumerations
### <a name='exseis-piol-io_driver-FileMode' /> public exseis::piol::io_driver::FileMode

The file modes possible for files. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Read | Read-only mode.  | = MPI_MODE_RDONLY | MPI_MODE_UNIQUE_OPEN |
| Write | Write-only mode.  | = MPI_MODE_CREATE | MPI_MODE_WRONLY | MPI_MODE_UNIQUE_OPEN |
| ReadWrite | Read or write.  | = MPI_MODE_CREATE | MPI_MODE_RDWR | MPI_MODE_UNIQUE_OPEN |



[Go to Top](#exseis-piol-io_driver)

[exseis-piol-io_driver-IO_driver]:./IO_driver.md
[exseis-piol-io_driver-IO_driver_mpi]:./IO_driver_mpi.md
