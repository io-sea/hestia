# <a name='exseis-piol-io_driver-IO_driver_mpi' /> public exseis::piol::io_driver::IO_driver_mpi

The MPI-IO Data class. 




## Inheritance:
Inherits from [exseis::piol::io_driver::IO_driver][exseis-piol-io_driver-IO_driver].

## Classes
| Name | Description | 
| ---- | ---- |
| [Opt](./IO_driver_mpi/Opt.md) | The MPI-IO options structure.  |


## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [IO_driver_mpi](#exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi) | Copy constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver_mpi-operator=) | Copy assignment: delete.  |
| [IO_driver_mpi](#exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-1) | Move constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver_mpi-operator=-1) | Move assignment: delete.  |


## Private Attributes
| Name | Description | 
| ---- | ---- |
| [m_log](#exseis-piol-io_driver-IO_driver_mpi-m_log) | Pointer to the PIOL object.  |
| [m_file_name](#exseis-piol-io_driver-IO_driver_mpi-m_file_name) | Store the file name for debugging purposes.  |
| [m_use_collective_operations](#exseis-piol-io_driver-IO_driver_mpi-m_use_collective_operations) | Whether collective read/write operations will be used.  |
| [m_file](#exseis-piol-io_driver-IO_driver_mpi-m_file) | The MPI-IO file handle.  |
| [m_file_communicator](#exseis-piol-io_driver-IO_driver_mpi-m_file_communicator) | The MPI communicator to use for file access.  |
| [m_info](#exseis-piol-io_driver-IO_driver_mpi-m_info) | The info structure to use.  |
| [m_max_size](#exseis-piol-io_driver-IO_driver_mpi-m_max_size) | The maximum size to allow to be written to disk per process in one operation.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [IO_driver_mpi](#exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-2) | The MPI-IO class constructor.  |
| [IO_driver_mpi](#exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-3) | The MPI-IO class constructor.  |
| [close](#exseis-piol-io_driver-IO_driver_mpi-close) | Explicit destructor, closes file and frees info.  |
| [~IO_driver_mpi](#exseis-piol-io_driver-IO_driver_mpi-~IO_driver_mpi) | Destructor.  |
| [is_open](#exseis-piol-io_driver-IO_driver_mpi-is_open) | Test if the file is open.  |
| [get_file_size](#exseis-piol-io_driver-IO_driver_mpi-get_file_size) | Get the size of the file.  |
| [set_file_size](#exseis-piol-io_driver-IO_driver_mpi-set_file_size) | Set the size of the file, either by truncating or expanding it.  |
| [read](#exseis-piol-io_driver-IO_driver_mpi-read) | Read a contiguous chunk of size `size` beginning a position `offset` from the file into the buffer `buffer`.  |
| [write](#exseis-piol-io_driver-IO_driver_mpi-write) | Write a contiguous chunk of size `size` beginning a position `offset` from the buffer `buffer` into the file.  |
| [read_noncontiguous](#exseis-piol-io_driver-IO_driver_mpi-read_noncontiguous) | Read a file in regularly spaced, non-contiguous blocks.  |
| [write_noncontiguous](#exseis-piol-io_driver-IO_driver_mpi-write_noncontiguous) | Write to a file in regularly spaced, non-contiguous blocks.  |
| [read_noncontiguous_irregular](#exseis-piol-io_driver-IO_driver_mpi-read_noncontiguous_irregular) | Read a file in irregularly spaced, non-contiguous chunks.  |
| [write_noncontiguous_irregular](#exseis-piol-io_driver-IO_driver_mpi-write_noncontiguous_irregular) | Write to a file in irregularly spaced, non-contiguous chunks.  |



## Special Member Functions
### <a name='exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi' /> public  exseis::piol::io_driver::IO_driver_mpi::IO_driver_mpi (const IO_driver_mpi &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-operator=' /> public [IO_driver_mpi][exseis-piol-io_driver-IO_driver_mpi] & exseis::piol::io_driver::IO_driver_mpi::operator= (const IO_driver_mpi &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [IO_driver_mpi][exseis-piol-io_driver-IO_driver_mpi] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-1' /> public  exseis::piol::io_driver::IO_driver_mpi::IO_driver_mpi (IO_driver_mpi &&)=delete

Move constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-operator=-1' /> public [IO_driver_mpi][exseis-piol-io_driver-IO_driver_mpi] & exseis::piol::io_driver::IO_driver_mpi::operator= (IO_driver_mpi &&)=delete

Move assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [IO_driver_mpi][exseis-piol-io_driver-IO_driver_mpi] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

## Private Attributes
### <a name='exseis-piol-io_driver-IO_driver_mpi-m_log' /> private exseis::piol::io_driver::IO_driver_mpi::m_log 

Pointer to the PIOL object. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_file_name' /> private exseis::piol::io_driver::IO_driver_mpi::m_file_name 

Store the file name for debugging purposes. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_use_collective_operations' /> private exseis::piol::io_driver::IO_driver_mpi::m_use_collective_operations 

Whether collective read/write operations will be used. 










Default value dependant on EXSEISDAT_MPIIO_COLLECTIVES definition. 




[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_file' /> private exseis::piol::io_driver::IO_driver_mpi::m_file  = MPI_FILE_NULL

The MPI-IO file handle. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_file_communicator' /> private exseis::piol::io_driver::IO_driver_mpi::m_file_communicator 

The MPI communicator to use for file access. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_info' /> private exseis::piol::io_driver::IO_driver_mpi::m_info 

The info structure to use. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-m_max_size' /> private exseis::piol::io_driver::IO_driver_mpi::m_max_size 

The maximum size to allow to be written to disk per process in one operation. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

## Public Functions
### <a name='exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-2' /> public  exseis::piol::io_driver::IO_driver_mpi::IO_driver_mpi (std::shared_ptr< ExSeisPIOL > piol, std::string file_name, FileMode mode, const IO_driver_mpi::Opt &opt=IO_driver_mpi::Opt())

The MPI-IO class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< ExSeisPIOL > | piol | The PIOL object, used for logging.  |
| in | std::string | file_name | The name of the file to operate on.  |
| in | [FileMode][exseis-piol-io_driver-FileMode] | mode | The file mode, e.g. read, write, delete on close etc.  |
| in | const [IO_driver_mpi::Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & | opt | The MPI-IO options  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-IO_driver_mpi-3' /> public  exseis::piol::io_driver::IO_driver_mpi::IO_driver_mpi (std::shared_ptr< exseis::utils::Log > log, std::string file_name, FileMode mode, const IO_driver_mpi::Opt &opt=IO_driver_mpi::Opt())

The MPI-IO class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< exseis::utils::Log > | log | The Log object to use for logging.  |
| in | std::string | file_name | The name of the file to operate on.  |
| in | [FileMode][exseis-piol-io_driver-FileMode] | mode | The file mode, e.g. read, write, delete on close etc.  |
| in | const [IO_driver_mpi::Opt][exseis-piol-io_driver-IO_driver_mpi-Opt] & | opt | The MPI-IO options  |












[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-close' /> public void exseis::piol::io_driver::IO_driver_mpi::close ()

Explicit destructor, closes file and frees info. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-~IO_driver_mpi' /> public  exseis::piol::io_driver::IO_driver_mpi::~IO_driver_mpi ()

Destructor. 








[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-is_open' /> public bool exseis::piol::io_driver::IO_driver_mpi::is_open () const override

Test if the file is open. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Returns `true` if the file is open, `false` otherwise.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-get_file_size' /> public size_t exseis::piol::io_driver::IO_driver_mpi::get_file_size () const override

Get the size of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The size of the file |









This function is collective over `file_communicator`. 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-set_file_size' /> public void exseis::piol::io_driver::IO_driver_mpi::set_file_size (size_t size) const override

Set the size of the file, either by truncating or expanding it. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | size | The new size of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-read' /> public void exseis::piol::io_driver::IO_driver_mpi::read (size_t offset, size_t size, void *buffer) const override

Read a contiguous chunk of size `size` beginning a position `offset` from the file into the buffer `buffer`. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The file offset to start reading at  |
| in | size_t | size | The amount to read  |
| out | void * | buffer | The buffer to read into (pointer to array of size `size`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-write' /> public void exseis::piol::io_driver::IO_driver_mpi::write (size_t offset, size_t size, const void *buffer) const override

Write a contiguous chunk of size `size` beginning a position `offset` from the buffer `buffer` into the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The file offset to start writing at  |
| in | size_t | size | The amount to write  |
| out | const void * | buffer | The buffer to write from (pointer to array of size `size`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-read_noncontiguous' /> public void exseis::piol::io_driver::IO_driver_mpi::read_noncontiguous (size_t offset, size_t block_size, size_t stride_size, size_t number_of_blocks, void *buffer) const override

Read a file in regularly spaced, non-contiguous blocks. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The position in the file to start reading from  |
| in | size_t | block_size | The block size to read in bytes  |
| in | size_t | stride_size | The stride size in bytes, i.e. the total size from the start of one block to the next  |
| in | size_t | number_of_blocks | The number of blocks to be read  |
| out | void * | buffer | Pointer to the buffer to read the data into (pointer to array of size `block_size * number_of_blocks`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-write_noncontiguous' /> public void exseis::piol::io_driver::IO_driver_mpi::write_noncontiguous (size_t offset, size_t block_size, size_t stride_size, size_t number_of_blocks, const void *buffer) const override

Write to a file in regularly spaced, non-contiguous blocks. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The position in the file to start writing to  |
| in | size_t | block_size | The block size to write in bytes  |
| in | size_t | stride_size | The stride size in bytes, i.e. the total size from the start of one block to the next  |
| in | size_t | number_of_blocks | The number of blocks to be written  |
| in | const void * | buffer | Pointer to the buffer to write the data from (pointer to array of size `block_size*number_of_blocks`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-read_noncontiguous_irregular' /> public void exseis::piol::io_driver::IO_driver_mpi::read_noncontiguous_irregular (size_t block_size, size_t number_of_blocks, const size_t *offsets, void *buffer) const override

Read a file in irregularly spaced, non-contiguous chunks. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | block_size | The block size to read in bytes  |
| in | size_t | number_of_blocks | The number of blocks to read  |
| in | const size_t * | offsets | Pointer to array of block offsets (size `number_of_blocks`)  |
| out | void * | buffer | Pointer to the buffer to read the data into (pointer to array of size `block_size*number_of_blocks`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

### <a name='exseis-piol-io_driver-IO_driver_mpi-write_noncontiguous_irregular' /> public void exseis::piol::io_driver::IO_driver_mpi::write_noncontiguous_irregular (size_t block_size, size_t number_of_blocks, const size_t *offsets, const void *buffer) const override

Write to a file in irregularly spaced, non-contiguous chunks. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | block_size | The block size to write in bytes  |
| in | size_t | number_of_blocks | The number of blocks to write  |
| in | const size_t * | offsets | Pointer to array of block offsets (size `number_of_blocks`)  |
| in | const void * | buffer | Pointer to the buffer to write the data from (pointer to array of size `block_size*number_of_blocks`)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver_mpi)

[exseis-piol-io_driver-FileMode]:./index.md#exseis-piol-io_driver-FileMode
[exseis-piol-io_driver-IO_driver]:./IO_driver.md
[exseis-piol-io_driver-IO_driver_mpi]:./IO_driver_mpi.md
[exseis-piol-io_driver-IO_driver_mpi-Opt]:./IO_driver_mpi/Opt.md#exseis-piol-io_driver-IO_driver_mpi-Opt
