# <a name='exseis-piol-file-Output_file' /> public exseis::piol::file::Output_file

The [`Output_file`][exseis-piol-file-Output_file] class is a generic interface for writing seismic data files. 




## Inheritance:
Is inherited by [exseis::piol::file::Output_file_segy][exseis-piol-file-Output_file_segy].

## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [Output_file](#exseis-piol-file-Output_file-Output_file) | Default constructor: default.  |
| [Output_file](#exseis-piol-file-Output_file-Output_file-1) | Copy constructor: delete.  |
| [operator=](#exseis-piol-file-Output_file-operator=) | Copy assignment: delete.  |
| [Output_file](#exseis-piol-file-Output_file-Output_file-2) | Move constructor: delete.  |
| [operator=](#exseis-piol-file-Output_file-operator=-1) | Move assignment: delete.  |
| [~Output_file](#exseis-piol-file-Output_file-~Output_file) | Virtual destructor.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [file_name](#exseis-piol-file-Output_file-file_name) | Get the name of the file.  |
| [write_text](#exseis-piol-file-Output_file-write_text) | Write the human readable text from the file.  |
| [write_ns](#exseis-piol-file-Output_file-write_ns) | Write the number of samples per trace.  |
| [write_nt](#exseis-piol-file-Output_file-write_nt) | Write the number of traces in the file.  |
| [write_sample_interval](#exseis-piol-file-Output_file-write_sample_interval) | Write the number of increment between trace samples.  |
| [write_param](#exseis-piol-file-Output_file-write_param) | Write the trace parameters from offset to offset+sz to the respective trace headers.  |
| [write_param_non_contiguous](#exseis-piol-file-Output_file-write_param_non_contiguous) | Write the parameters specified by the offsets in the passed offset array.  |
| [write_trace](#exseis-piol-file-Output_file-write_trace) | Write the traces from offset to offset+sz.  |
| [write_trace_non_contiguous](#exseis-piol-file-Output_file-write_trace_non_contiguous) | Write the traces specified by the offsets in the passed offset array.  |



## Special Member Functions
### <a name='exseis-piol-file-Output_file-Output_file' /> public  exseis::piol::file::Output_file::Output_file ()=default

Default constructor: default. 








[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-Output_file-1' /> public  exseis::piol::file::Output_file::Output_file (const Output_file &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-operator=' /> public [Output_file][exseis-piol-file-Output_file] & exseis::piol::file::Output_file::operator= (const Output_file &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Output_file][exseis-piol-file-Output_file] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-Output_file-2' /> public  exseis::piol::file::Output_file::Output_file (Output_file &&)=delete

Move constructor: delete. 








[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-operator=-1' /> public [Output_file][exseis-piol-file-Output_file] & exseis::piol::file::Output_file::operator= (Output_file &&)=delete

Move assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Output_file][exseis-piol-file-Output_file] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-~Output_file' /> public  exseis::piol::file::Output_file::~Output_file ()

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

## Public Functions
### <a name='exseis-piol-file-Output_file-file_name' /> public const std::string & exseis::piol::file::Output_file::file_name () const =0

Get the name of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | The name of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_text' /> public void exseis::piol::file::Output_file::write_text (std::string text)=0

Write the human readable text from the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | text | The new string containing the text (in ASCII format).  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_ns' /> public void exseis::piol::file::Output_file::write_ns (size_t ns)=0

Write the number of samples per trace. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | ns | The new number of samples per trace.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_nt' /> public void exseis::piol::file::Output_file::write_nt (size_t nt)=0

Write the number of traces in the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | The new number of traces.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_sample_interval' /> public void exseis::piol::file::Output_file::write_sample_interval (exseis::utils::Floating_point sample_interval)=0

Write the number of increment between trace samples. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Floating_point | sample_interval | The new increment between trace samples.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_param' /> public void exseis::piol::file::Output_file::write_param (size_t offset, size_t sz, const Trace_metadata *prm, size_t skip=0)=0

Write the trace parameters from offset to offset+sz to the respective trace headers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| in | const Trace_metadata * | prm | An array of the parameter structures (size sizeof(Trace_metadata)*sz)  |
| in | size_t | skip | When writing, skip the first "skip" entries of prm |








It is assumed that this operation is not an update. Any previous contents of the trace header will be overwritten. 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_param_non_contiguous' /> public void exseis::piol::file::Output_file::write_param_non_contiguous (size_t sz, const size_t *offset, const Trace_metadata *prm, size_t skip=0)=0

Write the parameters specified by the offsets in the passed offset array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | An array of trace numbers to write.  |
| in | const Trace_metadata * | prm | A parameter structure  |
| in | size_t | skip | When writing, skip the first "skip" entries of prm |








It is assumed that the parameter writing operation is not an update. Any previous contents of the trace header will be overwritten. 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_trace' /> public void exseis::piol::file::Output_file::write_trace (size_t offset, size_t sz, exseis::utils::Trace_value *trace, const Trace_metadata *prm=nullptr, size_t skip=0)=0

Write the traces from offset to offset+sz. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| in | exseis::utils::Trace_value * | trace | The array of traces to write to the file  |
| in | const Trace_metadata * | prm | A contiguous array of the parameter structures (size sizeof(Trace_metadata)*sz)  |
| in | size_t | skip | When writing, skip the first "skip" entries of prm  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

### <a name='exseis-piol-file-Output_file-write_trace_non_contiguous' /> public void exseis::piol::file::Output_file::write_trace_non_contiguous (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, const Trace_metadata *prm=nullptr, size_t skip=0)=0

Write the traces specified by the offsets in the passed offset array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | An array of trace numbers to write.  |
| in | exseis::utils::Trace_value * | trace | A contiguous array of each trace (size sz*ns*sizeof(exseis::utils::Trace_value))  |
| in | const Trace_metadata * | prm | A parameter structure  |
| in | size_t | skip | When writing, skip the first "skip" entries of prm |








When prm==nullptr only the trace data is written. It is assumed that the parameter writing operation is not an update. Any previous contents of the trace header will be overwritten. 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file)

[exseis-piol-file-Output_file]:./Output_file.md
[exseis-piol-file-Output_file_segy]:./Output_file_segy.md
