# <a name='exseis-piol-io_driver-IO_driver' /> public exseis::piol::io_driver::IO_driver

The Data layer interface. Specific data I/O implementations work off this base class. 




## Inheritance:
Is inherited by [exseis::piol::io_driver::IO_driver_mpi][exseis-piol-io_driver-IO_driver_mpi].

## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [IO_driver](#exseis-piol-io_driver-IO_driver-IO_driver) | Default constructor: default.  |
| [~IO_driver](#exseis-piol-io_driver-IO_driver-~IO_driver) | Virtual destructor.  |
| [IO_driver](#exseis-piol-io_driver-IO_driver-IO_driver-1) | Copy constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver-operator=) | Copy assignment: delete.  |
| [IO_driver](#exseis-piol-io_driver-IO_driver-IO_driver-2) | Move constructor: delete.  |
| [operator=](#exseis-piol-io_driver-IO_driver-operator=-1) | Move assignment: delete.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [is_open](#exseis-piol-io_driver-IO_driver-is_open) | Test if the file is open.  |
| [get_file_size](#exseis-piol-io_driver-IO_driver-get_file_size) | Get the size of the file.  |
| [set_file_size](#exseis-piol-io_driver-IO_driver-set_file_size) | Set the size of the file, either by truncating or expanding it.  |
| [read](#exseis-piol-io_driver-IO_driver-read) | Read a contiguous chunk of size `size` beginning a position `offset` from the file into the buffer `buffer`.  |
| [write](#exseis-piol-io_driver-IO_driver-write) | Write a contiguous chunk of size `size` beginning a position `offset` from the buffer `buffer` into the file.  |
| [read_noncontiguous](#exseis-piol-io_driver-IO_driver-read_noncontiguous) | Read a file in regularly spaced, non-contiguous blocks.  |
| [write_noncontiguous](#exseis-piol-io_driver-IO_driver-write_noncontiguous) | Write to a file in regularly spaced, non-contiguous blocks.  |
| [read_noncontiguous_irregular](#exseis-piol-io_driver-IO_driver-read_noncontiguous_irregular) | Read a file in irregularly spaced, non-contiguous chunks.  |
| [write_noncontiguous_irregular](#exseis-piol-io_driver-IO_driver-write_noncontiguous_irregular) | Write to a file in irregularly spaced, non-contiguous chunks.  |



## Special Member Functions
### <a name='exseis-piol-io_driver-IO_driver-IO_driver' /> public  exseis::piol::io_driver::IO_driver::IO_driver ()=default

Default constructor: default. 








[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-~IO_driver' /> public  exseis::piol::io_driver::IO_driver::~IO_driver ()=default

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-IO_driver-1' /> public  exseis::piol::io_driver::IO_driver::IO_driver (const IO_driver &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-operator=' /> public [IO_driver][exseis-piol-io_driver-IO_driver] & exseis::piol::io_driver::IO_driver::operator= (const IO_driver &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [IO_driver][exseis-piol-io_driver-IO_driver] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-IO_driver-2' /> public  exseis::piol::io_driver::IO_driver::IO_driver (IO_driver &&)=delete

Move constructor: delete. 








[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-operator=-1' /> public [IO_driver][exseis-piol-io_driver-IO_driver] & exseis::piol::io_driver::IO_driver::operator= (IO_driver &&)=delete

Move assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [IO_driver][exseis-piol-io_driver-IO_driver] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-io_driver-IO_driver)

## Public Functions
### <a name='exseis-piol-io_driver-IO_driver-is_open' /> public bool exseis::piol::io_driver::IO_driver::is_open () const =0

Test if the file is open. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Returns `true` if the file is open, `false` otherwise.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-get_file_size' /> public size_t exseis::piol::io_driver::IO_driver::get_file_size () const =0

Get the size of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The size of the file  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-set_file_size' /> public void exseis::piol::io_driver::IO_driver::set_file_size (size_t size) const =0

Set the size of the file, either by truncating or expanding it. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | size | The new size of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-read' /> public void exseis::piol::io_driver::IO_driver::read (size_t offset, size_t size, void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-write' /> public void exseis::piol::io_driver::IO_driver::write (size_t offset, size_t size, const void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-read_noncontiguous' /> public void exseis::piol::io_driver::IO_driver::read_noncontiguous (size_t offset, size_t block_size, size_t stride_size, size_t number_of_blocks, void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-write_noncontiguous' /> public void exseis::piol::io_driver::IO_driver::write_noncontiguous (size_t offset, size_t block_size, size_t stride_size, size_t number_of_blocks, const void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-read_noncontiguous_irregular' /> public void exseis::piol::io_driver::IO_driver::read_noncontiguous_irregular (size_t block_size, size_t number_of_blocks, const size_t *offsets, void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

### <a name='exseis-piol-io_driver-IO_driver-write_noncontiguous_irregular' /> public void exseis::piol::io_driver::IO_driver::write_noncontiguous_irregular (size_t block_size, size_t number_of_blocks, const size_t *offsets, const void *buffer) const =0

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


[Go to Top](#exseis-piol-io_driver-IO_driver)

[exseis-piol-io_driver-IO_driver]:./IO_driver.md
[exseis-piol-io_driver-IO_driver_mpi]:./IO_driver_mpi.md
