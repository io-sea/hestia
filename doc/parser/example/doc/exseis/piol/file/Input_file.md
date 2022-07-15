# <a name='exseis-piol-file-Input_file' /> public exseis::piol::file::Input_file

The [`Input_file`][exseis-piol-file-Input_file] class is a generic interface for reading seismic data files. 




## Inheritance:
Is inherited by [exseis::piol::file::Input_file_segy][exseis-piol-file-Input_file_segy].

## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [Input_file](#exseis-piol-file-Input_file-Input_file) | Default constructor: default.  |
| [~Input_file](#exseis-piol-file-Input_file-~Input_file) | Virtual destructor.  |
| [Input_file](#exseis-piol-file-Input_file-Input_file-1) | Copy constructor: delete.  |
| [operator=](#exseis-piol-file-Input_file-operator=) | Copy assignment: delete.  |
| [Input_file](#exseis-piol-file-Input_file-Input_file-2) | Move constructor: delete.  |
| [operator=](#exseis-piol-file-Input_file-operator=-1) | Move assignment: delete.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [file_name](#exseis-piol-file-Input_file-file_name) | Get the name of the file.  |
| [read_file_headers](#exseis-piol-file-Input_file-read_file_headers) | Read the global file headers from the file.  |
| [read_text](#exseis-piol-file-Input_file-read_text) | Read the human readable text from the file.  |
| [read_ns](#exseis-piol-file-Input_file-read_ns) | Read the number of samples per trace.  |
| [read_nt](#exseis-piol-file-Input_file-read_nt) | Read the number of traces in the file.  |
| [read_sample_interval](#exseis-piol-file-Input_file-read_sample_interval) | Read the number of increment between trace samples.  |
| [read_param](#exseis-piol-file-Input_file-read_param) | Read the trace parameters from offset to offset+sz of the respective trace headers.  |
| [read_param_non_contiguous](#exseis-piol-file-Input_file-read_param_non_contiguous) | Read the traces specified by the offsets in the passed offset array.  |
| [read_trace](#exseis-piol-file-Input_file-read_trace) | Read the traces from offset to offset+sz.  |
| [read_trace_non_contiguous](#exseis-piol-file-Input_file-read_trace_non_contiguous) | Read the traces specified by the offsets in the passed offset array. Assumes Monotonic.  |
| [read_trace_non_monotonic](#exseis-piol-file-Input_file-read_trace_non_monotonic) | Read the traces specified by the offsets in the passed offset array. Does not assume monotonic.  |



## Special Member Functions
### <a name='exseis-piol-file-Input_file-Input_file' /> public  exseis::piol::file::Input_file::Input_file ()=default

Default constructor: default. 








[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-~Input_file' /> public  exseis::piol::file::Input_file::~Input_file ()

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-Input_file-1' /> public  exseis::piol::file::Input_file::Input_file (const Input_file &)=delete

Copy constructor: delete. 








[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-operator=' /> public [Input_file][exseis-piol-file-Input_file] & exseis::piol::file::Input_file::operator= (const Input_file &)=delete

Copy assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Input_file][exseis-piol-file-Input_file] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-Input_file-2' /> public  exseis::piol::file::Input_file::Input_file (Input_file &&)=delete

Move constructor: delete. 








[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-operator=-1' /> public [Input_file][exseis-piol-file-Input_file] & exseis::piol::file::Input_file::operator= (Input_file &&)=delete

Move assignment: delete. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Input_file][exseis-piol-file-Input_file] & | A reference to the current instance.  |












[Go to Top](#exseis-piol-file-Input_file)

## Public Functions
### <a name='exseis-piol-file-Input_file-file_name' /> public const std::string & exseis::piol::file::Input_file::file_name () const =0

Get the name of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | The name of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_file_headers' /> public void exseis::piol::file::Input_file::read_file_headers ()=0

Read the global file headers from the file. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_text' /> public const std::string & exseis::piol::file::Input_file::read_text () const =0

Read the human readable text from the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | A string containing the text (in ASCII format)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_ns' /> public size_t exseis::piol::file::Input_file::read_ns () const =0

Read the number of samples per trace. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of samples per trace  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_nt' /> public size_t exseis::piol::file::Input_file::read_nt () const =0

Read the number of traces in the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of traces  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_sample_interval' /> public exseis::utils::Floating_point exseis::piol::file::Input_file::read_sample_interval () const =0

Read the number of increment between trace samples. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Floating_point | The increment between trace samples  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_param' /> public void exseis::piol::file::Input_file::read_param (size_t offset, size_t sz, Trace_metadata *prm, size_t skip=0) const =0

Read the trace parameters from offset to offset+sz of the respective trace headers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| in | Trace_metadata * | prm | An array of the parameter structures (size sizeof(Trace_metadata)*sz)  |
| in | size_t | skip | When reading, skip the first "skip" entries of prm  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_param_non_contiguous' /> public void exseis::piol::file::Input_file::read_param_non_contiguous (size_t sz, const size_t *offset, Trace_metadata *prm, size_t skip=0) const =0

Read the traces specified by the offsets in the passed offset array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | An array of trace numbers to read.  |
| out | Trace_metadata * | prm | A parameter structure  |
| in | size_t | skip | When reading, skip the first "skip" entries of prm  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_trace' /> public void exseis::piol::file::Input_file::read_trace (size_t offset, size_t sz, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const =0

Read the traces from offset to offset+sz. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| out | exseis::utils::Trace_value * | trace | The array of traces to fill from the file  |
| out | Trace_metadata * | prm | A contiguous array of the parameter structures (size sizeof(Trace_metadata)*sz)  |
| in | size_t | skip | When reading, skip the first "skip" entries of prm  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_trace_non_contiguous' /> public void exseis::piol::file::Input_file::read_trace_non_contiguous (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const =0

Read the traces specified by the offsets in the passed offset array. Assumes Monotonic. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | An array of trace numbers to read (monotonic list).  |
| out | exseis::utils::Trace_value * | trace | A contiguous array of each trace (size sz*ns*sizeof(exseis::utils::Trace_value))  |
| out | Trace_metadata * | prm | A parameter structure  |
| in | size_t | skip | When reading, skip the first "skip" entries of prm |








When prm==nullptr only the trace data is read. 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

### <a name='exseis-piol-file-Input_file-read_trace_non_monotonic' /> public void exseis::piol::file::Input_file::read_trace_non_monotonic (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const =0

Read the traces specified by the offsets in the passed offset array. Does not assume monotonic. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | An array of trace numbers to read (non-monotonic list).  |
| out | exseis::utils::Trace_value * | trace | A contiguous array of each trace (size sz*ns*sizeof(exseis::utils::Trace_value))  |
| out | Trace_metadata * | prm | A parameter structure  |
| in | size_t | skip | When reading, skip the first "skip" entries of prm |








When prm==nullptr only the trace data is read. 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file)

[exseis-piol-file-Input_file]:./Input_file.md
[exseis-piol-file-Input_file_segy]:./Input_file_segy.md
