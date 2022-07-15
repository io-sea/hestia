# <a name='exseis-utils' />  exseis::utils

Utility classes and functions. 



The [exseis::utils][exseis-utils] class contains utility functions for parallel programming, seismic data processing, converting between data types etc. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Enum_hash](./Enum_hash.md) | A hashing function for enums and enum classes.  |


## Namespaces
| Name | Description | 
| ---- | ---- |
| [communicator](./communicator/index.md) | Interface and implementations of a communicator that is used for message passing between the processes.  |
| [decomposition](./decomposition/index.md) | Functions for splitting ranges across processes.  |
| [distributed_vector](./distributed_vector/index.md) | Interface and implementations of a vector that is distributed over several processes.  |
| [encoding](./encoding/index.md) | Functions for different kinds of encodings.  |
| [generic_array](./generic_array/index.md) | The [Generic_array][exseis-utils-generic_array-Generic_array] class and related functions, for providing an interface using one type, and underlying storage of another.  |
| [logging](./logging/index.md) | Logging utilities.  |
| [mpi_utils](./mpi_utils/index.md) | MPI utilities.  |
| [signal_processing](./signal_processing/index.md) | Functions for processing seismic signals.  |
| [types](./types/index.md) | Aliases for fundamental types and conversions used throughout the ExSeisDat project.  |



[exseis-utils]:./index.md
[exseis-utils-generic_array-Generic_array]:./generic_array/Generic_array.md
