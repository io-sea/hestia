# <a name='exseis-piol-file-Input_file_segy' /> public exseis::piol::file::Input_file_segy

The SEG-Y implementation of the file layer. 




## Inheritance:
Is inherited by exseis::piol::file::detail::ReadSEGYModel.

## Classes
| Name | Description | 
| ---- | ---- |
| [Options](./Input_file_segy/Options.md) | The SEG-Y options structure.  |


## Protected Attributes
| Name | Description | 
| ---- | ---- |
| [m_piol](#exseis-piol-file-Input_file_segy-m_piol) | The PIOL object.  |
| [m_name](#exseis-piol-file-Input_file_segy-m_name) | Store the file name for debugging purposes.  |
| [m_obj](#exseis-piol-file-Input_file_segy-m_obj) | Pointer to the Object-layer object (polymorphic).  |
| [m_ns](#exseis-piol-file-Input_file_segy-m_ns) | The number of samples per trace.  |
| [m_nt](#exseis-piol-file-Input_file_segy-m_nt) | The number of traces.  |
| [m_text](#exseis-piol-file-Input_file_segy-m_text) | Human readable text extracted from the file.  |
| [m_sample_interval](#exseis-piol-file-Input_file_segy-m_sample_interval) | The interval between samples in a trace.  |
| [m_number_format](#exseis-piol-file-Input_file_segy-m_number_format) | Type formats.  |
| [m_sample_interval_factor](#exseis-piol-file-Input_file_segy-m_sample_interval_factor) | The interval factor.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Input_file_segy](#exseis-piol-file-Input_file_segy-Input_file_segy) | The SEGY-Object class constructor.  |
| [file_name](#exseis-piol-file-Input_file_segy-file_name) | Get the name of the file.  |
| [read_file_headers](#exseis-piol-file-Input_file_segy-read_file_headers) | Read the global file headers from the file.  |
| [read_text](#exseis-piol-file-Input_file_segy-read_text) | Read the human readable text from the file.  |
| [read_ns](#exseis-piol-file-Input_file_segy-read_ns) | Read the number of samples per trace.  |
| [read_nt](#exseis-piol-file-Input_file_segy-read_nt) | Read the number of traces in the file.  |
| [read_sample_interval](#exseis-piol-file-Input_file_segy-read_sample_interval) | Read the number of increment between trace samples.  |
| [read_param](#exseis-piol-file-Input_file_segy-read_param) | Read the trace parameters from offset to offset+sz of the respective trace headers.  |
| [read_param_non_contiguous](#exseis-piol-file-Input_file_segy-read_param_non_contiguous) | Read the traces specified by the offsets in the passed offset array.  |
| [read_trace](#exseis-piol-file-Input_file_segy-read_trace) | Read the traces from offset to offset+sz.  |
| [read_trace_non_contiguous](#exseis-piol-file-Input_file_segy-read_trace_non_contiguous) | Read the traces specified by the offsets in the passed offset array. Assumes Monotonic.  |
| [read_trace_non_monotonic](#exseis-piol-file-Input_file_segy-read_trace_non_monotonic) | Read the traces specified by the offsets in the passed offset array. Does not assume monotonic.  |


## Protected Functions
| Name | Description | 
| ---- | ---- |
| [Input_file_segy](#exseis-piol-file-Input_file_segy-Input_file_segy-1) | The SEGY-Object class constructor.  |



## Protected Attributes
### <a name='exseis-piol-file-Input_file_segy-m_piol' /> protected exseis::piol::file::Input_file_segy::m_piol 

The PIOL object. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_name' /> protected exseis::piol::file::Input_file_segy::m_name  = ""

Store the file name for debugging purposes. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_obj' /> protected exseis::piol::file::Input_file_segy::m_obj 

Pointer to the Object-layer object (polymorphic). 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_ns' /> protected exseis::piol::file::Input_file_segy::m_ns  = 0

The number of samples per trace. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_nt' /> protected exseis::piol::file::Input_file_segy::m_nt  = 0

The number of traces. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_text' /> protected exseis::piol::file::Input_file_segy::m_text  = ""

Human readable text extracted from the file. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_sample_interval' /> protected exseis::piol::file::Input_file_segy::m_sample_interval  = 0

The interval between samples in a trace. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_number_format' /> protected exseis::piol::file::Input_file_segy::m_number_format  = 

Type formats. 








[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-m_sample_interval_factor' /> protected exseis::piol::file::Input_file_segy::m_sample_interval_factor 

The interval factor. 








[Go to Top](#exseis-piol-file-Input_file_segy)

## Public Functions
### <a name='exseis-piol-file-Input_file_segy-Input_file_segy' /> public  exseis::piol::file::Input_file_segy::Input_file_segy (std::shared_ptr< ExSeisPIOL > piol, std::string name, const Input_file_segy::Options &options=Input_file_segy::Options())

The SEGY-Object class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< ExSeisPIOL > | piol | This PIOL ptr is not modified but is used to instantiate another shared_ptr.  |
| in | std::string | name | The name of the file associated with the instantiation.  |
| in | const [Input_file_segy::Options][exseis-piol-file-Input_file_segy-Options] & | options | The SEGY-File options  |












[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-file_name' /> public const std::string & exseis::piol::file::Input_file_segy::file_name () const override

Get the name of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | The name of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_file_headers' /> public void exseis::piol::file::Input_file_segy::read_file_headers () override

Read the global file headers from the file. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_text' /> public const std::string & exseis::piol::file::Input_file_segy::read_text () const override

Read the human readable text from the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | A string containing the text (in ASCII format)  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_ns' /> public size_t exseis::piol::file::Input_file_segy::read_ns () const override

Read the number of samples per trace. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of samples per trace  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_nt' /> public size_t exseis::piol::file::Input_file_segy::read_nt () const override

Read the number of traces in the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of traces  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_sample_interval' /> public exseis::utils::Floating_point exseis::piol::file::Input_file_segy::read_sample_interval () const override

Read the number of increment between trace samples. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Floating_point | The increment between trace samples  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_param' /> public void exseis::piol::file::Input_file_segy::read_param (size_t offset, size_t sz, Trace_metadata *prm, size_t skip=0) const override

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


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_param_non_contiguous' /> public void exseis::piol::file::Input_file_segy::read_param_non_contiguous (size_t sz, const size_t *offset, Trace_metadata *prm, size_t skip=0) const override

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


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_trace' /> public void exseis::piol::file::Input_file_segy::read_trace (size_t offset, size_t sz, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const override

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


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_trace_non_contiguous' /> public void exseis::piol::file::Input_file_segy::read_trace_non_contiguous (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const override

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


[Go to Top](#exseis-piol-file-Input_file_segy)

### <a name='exseis-piol-file-Input_file_segy-read_trace_non_monotonic' /> public void exseis::piol::file::Input_file_segy::read_trace_non_monotonic (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, Trace_metadata *prm=nullptr, size_t skip=0) const override

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


[Go to Top](#exseis-piol-file-Input_file_segy)

## Protected Functions
### <a name='exseis-piol-file-Input_file_segy-Input_file_segy-1' /> protected  exseis::piol::file::Input_file_segy::Input_file_segy (std::shared_ptr< ExSeisPIOL > piol, std::string name, const Input_file_segy::Options &options, std::shared_ptr< ObjectInterface > object)

The SEGY-Object class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< ExSeisPIOL > | piol | This PIOL ptr is not modified but is used to instantiate another shared_ptr.  |
| in | std::string | name | The name of the file associated with the instantiation.  |
| in | const [Input_file_segy::Options][exseis-piol-file-Input_file_segy-Options] & | options | The SEGY-File options  |
| in | std::shared_ptr< ObjectInterface > | object | A shared pointer to the object layer  |












[Go to Top](#exseis-piol-file-Input_file_segy)

[exseis-piol-file-Input_file]:./Input_file.md
[exseis-piol-file-Input_file_segy-Options]:./Input_file_segy/Options.md#exseis-piol-file-Input_file_segy-Options
[exseis-piol-segy-Segy_number_format]:./../segy/index.md#exseis-piol-segy-Segy_number_format
