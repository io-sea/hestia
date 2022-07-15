# <a name='exseis-utils-communicator' />  exseis::utils::communicator

Interface and implementations of a communicator that is used for message passing between the processes. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Communicator](./Communicator.md) | The Communication layer interface. Specific communication implementations work off this base class.  |
| [Communicator_mpi](./Communicator_mpi.md) | The MPI communication class. All MPI communication specific routines should be wrapped up and accessible from this class.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [manage_mpi](#exseis-utils-communicator-manage_mpi) | Set whether PIOL should manage MPI initialization / finalization. By default, PIOL will manage MPI if it calls MPI_Init, and it will call MPI_Init if the piol::Communicator_mpi class is initialized before MPI_Init is called. If PIOL Is managing MPI, it will call MPI_Finalize on program exit.  |



## Functions
### <a name='exseis-utils-communicator-manage_mpi' /> public void exseis::utils::communicator::manage_mpi (bool)

Set whether PIOL should manage MPI initialization / finalization. By default, PIOL will manage MPI if it calls MPI_Init, and it will call MPI_Init if the piol::Communicator_mpi class is initialized before MPI_Init is called. If PIOL Is managing MPI, it will call MPI_Finalize on program exit. 








[Go to Top](#exseis-utils-communicator)

[exseis-utils-communicator-Communicator]:./Communicator.md
[exseis-utils-communicator-Communicator_mpi]:./Communicator_mpi.md
