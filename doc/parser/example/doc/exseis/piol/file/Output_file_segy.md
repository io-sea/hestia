# <a name='exseis-piol-file-Output_file_segy' /> public exseis::piol::file::Output_file_segy

The SEG-Y implementation of the file layer. 




## Inheritance:
Inherits from [exseis::piol::file::Output_file][exseis-piol-file-Output_file].

## Classes
| Name | Description | 
| ---- | ---- |
| [Flags](./Output_file_segy/Flags.md) | State flags structure for SEGY.  |
| [Options](./Output_file_segy/Options.md) | The SEG-Y options structure.  |


## Protected Attributes
| Name | Description | 
| ---- | ---- |
| [m_piol](#exseis-piol-file-Output_file_segy-m_piol) | The PIOL object.  |
| [m_name](#exseis-piol-file-Output_file_segy-m_name) | Store the file name for debugging purposes.  |
| [m_obj](#exseis-piol-file-Output_file_segy-m_obj) | Pointer to the Object-layer object (polymorphic).  |
| [m_ns](#exseis-piol-file-Output_file_segy-m_ns) | The number of samples per trace.  |
| [m_nt](#exseis-piol-file-Output_file_segy-m_nt) | The number of traces.  |
| [m_text](#exseis-piol-file-Output_file_segy-m_text) | Human readable text extracted from the file.  |
| [m_sample_interval](#exseis-piol-file-Output_file_segy-m_sample_interval) | The increment between samples in a trace.  |
| [m_is_ns_set](#exseis-piol-file-Output_file_segy-m_is_ns_set) | Whether the number of samples (ns) has been set.  |
| [m_state](#exseis-piol-file-Output_file_segy-m_state) | State flags are stored in this structure.  |
| [m_sample_interval_factor](#exseis-piol-file-Output_file_segy-m_sample_interval_factor) | The increment factor.  |


## Protected Functions
| Name | Description | 
| ---- | ---- |
| [calc_nt](#exseis-piol-file-Output_file_segy-calc_nt) | Calculate the number of traces currently stored (or implied to exist).  |
| [Output_file_segy](#exseis-piol-file-Output_file_segy-Output_file_segy) | The SEGY-Object class constructor.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Output_file_segy](#exseis-piol-file-Output_file_segy-Output_file_segy-1) | The SEGY-Object class constructor.  |
| [~Output_file_segy](#exseis-piol-file-Output_file_segy-~Output_file_segy) | Destructor. Processes any remaining flags.  |
| [flush](#exseis-piol-file-Output_file_segy-flush) | Flush. Processes any remaining flags.  |
| [file_name](#exseis-piol-file-Output_file_segy-file_name) | Get the name of the file.  |
| [write_text](#exseis-piol-file-Output_file_segy-write_text) | Write the human readable text from the file.  |
| [write_ns](#exseis-piol-file-Output_file_segy-write_ns) | Write the number of samples per trace.  |
| [write_nt](#exseis-piol-file-Output_file_segy-write_nt) | Write the number of traces in the file.  |
| [write_sample_interval](#exseis-piol-file-Output_file_segy-write_sample_interval) | Write the number of increment between trace samples.  |
| [write_param](#exseis-piol-file-Output_file_segy-write_param) | Write the trace parameters from offset to offset+sz to the respective trace headers.  |
| [write_param_non_contiguous](#exseis-piol-file-Output_file_segy-write_param_non_contiguous) | Write the parameters specified by the offsets in the passed offset array.  |
| [write_trace](#exseis-piol-file-Output_file_segy-write_trace) | Write the traces from offset to offset+sz.  |
| [write_trace_non_contiguous](#exseis-piol-file-Output_file_segy-write_trace_non_contiguous) | Write the traces specified by the offsets in the passed offset array.  |



## Protected Attributes
### <a name='exseis-piol-file-Output_file_segy-m_piol' /> protected exseis::piol::file::Output_file_segy::m_piol 

The PIOL object. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_name' /> protected exseis::piol::file::Output_file_segy::m_name 

Store the file name for debugging purposes. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_obj' /> protected exseis::piol::file::Output_file_segy::m_obj 

Pointer to the Object-layer object (polymorphic). 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_ns' /> protected exseis::piol::file::Output_file_segy::m_ns  = 0

The number of samples per trace. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_nt' /> protected exseis::piol::file::Output_file_segy::m_nt  = 0

The number of traces. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_text' /> protected exseis::piol::file::Output_file_segy::m_text  = ""

Human readable text extracted from the file. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_sample_interval' /> protected exseis::piol::file::Output_file_segy::m_sample_interval  = 0

The increment between samples in a trace. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_is_ns_set' /> protected exseis::piol::file::Output_file_segy::m_is_ns_set  = false

Whether the number of samples (ns) has been set. 










If it hasn't been set before file writing, the output is likely garbage. 




[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_state' /> protected exseis::piol::file::Output_file_segy::m_state  = {}

State flags are stored in this structure. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-m_sample_interval_factor' /> protected exseis::piol::file::Output_file_segy::m_sample_interval_factor 

The increment factor. 








[Go to Top](#exseis-piol-file-Output_file_segy)

## Protected Functions
### <a name='exseis-piol-file-Output_file_segy-calc_nt' /> protected size_t exseis::piol::file::Output_file_segy::calc_nt ()

Calculate the number of traces currently stored (or implied to exist). 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the number of traces  |












[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-Output_file_segy' /> protected  exseis::piol::file::Output_file_segy::Output_file_segy (std::shared_ptr< ExSeisPIOL > piol, std::string name, const Output_file_segy::Options &options, std::shared_ptr< ObjectInterface > obj)

The SEGY-Object class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< ExSeisPIOL > | piol | This PIOL ptr is not modified but is used to instantiate another shared_ptr.  |
| in | std::string | name | The name of the file associated with the instantiation.  |
| in | const [Output_file_segy::Options][exseis-piol-file-Output_file_segy-Options] & | options | The SEGY-File options  |
| in | std::shared_ptr< ObjectInterface > | obj | The object layer object to use.  |












[Go to Top](#exseis-piol-file-Output_file_segy)

## Public Functions
### <a name='exseis-piol-file-Output_file_segy-Output_file_segy-1' /> public  exseis::piol::file::Output_file_segy::Output_file_segy (std::shared_ptr< ExSeisPIOL > piol, std::string name, const Output_file_segy::Options &options=Output_file_segy::Options())

The SEGY-Object class constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< ExSeisPIOL > | piol | This PIOL ptr is not modified but is used to instantiate another shared_ptr.  |
| in | std::string | name | The name of the file associated with the instantiation.  |
| in | const [Output_file_segy::Options][exseis-piol-file-Output_file_segy-Options] & | options | The SEGY-File options  |












[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-~Output_file_segy' /> public  exseis::piol::file::Output_file_segy::~Output_file_segy ()

Destructor. Processes any remaining flags. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-flush' /> public void exseis::piol::file::Output_file_segy::flush ()

Flush. Processes any remaining flags. 








[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-file_name' /> public const std::string & exseis::piol::file::Output_file_segy::file_name () const override

Get the name of the file. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const std::string & | The name of the file.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_text' /> public void exseis::piol::file::Output_file_segy::write_text (std::string text) override

Write the human readable text from the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | text | The new string containing the text (in ASCII format).  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_ns' /> public void exseis::piol::file::Output_file_segy::write_ns (size_t ns) override

Write the number of samples per trace. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | ns | The new number of samples per trace.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_nt' /> public void exseis::piol::file::Output_file_segy::write_nt (size_t nt) override

Write the number of traces in the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | The new number of traces.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_sample_interval' /> public void exseis::piol::file::Output_file_segy::write_sample_interval (exseis::utils::Floating_point sample_interval) override

Write the number of increment between trace samples. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Floating_point | sample_interval | The new increment between trace samples.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_param' /> public void exseis::piol::file::Output_file_segy::write_param (size_t offset, size_t sz, const Trace_metadata *prm, size_t skip=0) override

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


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_param_non_contiguous' /> public void exseis::piol::file::Output_file_segy::write_param_non_contiguous (size_t sz, const size_t *offset, const Trace_metadata *prm, size_t skip=0) override

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


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_trace' /> public void exseis::piol::file::Output_file_segy::write_trace (size_t offset, size_t sz, exseis::utils::Trace_value *trace, const Trace_metadata *prm=nullptr, size_t skip=0) override

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


[Go to Top](#exseis-piol-file-Output_file_segy)

### <a name='exseis-piol-file-Output_file_segy-write_trace_non_contiguous' /> public void exseis::piol::file::Output_file_segy::write_trace_non_contiguous (size_t sz, const size_t *offset, exseis::utils::Trace_value *trace, const Trace_metadata *prm=nullptr, size_t skip=0) override

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


[Go to Top](#exseis-piol-file-Output_file_segy)

[exseis-piol-file-Output_file]:./Output_file.md
[exseis-piol-file-Output_file_segy-Flags]:./Output_file_segy/Flags.md#exseis-piol-file-Output_file_segy-Flags
[exseis-piol-file-Output_file_segy-Options]:./Output_file_segy/Options.md#exseis-piol-file-Output_file_segy-Options
