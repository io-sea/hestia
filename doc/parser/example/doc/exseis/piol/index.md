# <a name='exseis-piol' />  exseis::piol

ExSeisPIOL: A High-Performance Parallel Seismic file I/O library. 



The ExSeisPIOL library is an **Ex**treme-scale **Seis**mic **P**arallel **I/O** **L**ibrary. It primarily handles high-performance, parallel I/O of seismic data files. 




## Namespaces
| Name | Description | 
| ---- | ---- |
| [configuration](./configuration/index.md) | Classes for storing configurations of the program.  |
| [file](./file/index.md) | Basic read and write classes.  |
| [io_driver](./io_driver/index.md) | Interface and implementations that perform I/O operations.  |
| [metadata](./metadata/index.md) | Seismic metadata utilities.  |
| [operations](./operations/index.md) | Basic operations performed on the file data.  |
| [segy](./segy/index.md) | SEGY specific parameters and functions.  |


## PIOL Set functions
| Name | Description | 
| ---- | ---- |
| [piol_set_new](#exseis-piol-piol_set_new) | Initialise the set.  |
| [piol_set_delete](#exseis-piol-piol_set_delete) | Free (deinit) the set.  |
| [piol_set_get_min_max](#exseis-piol-piol_set_get_min_max) | Get the min and the max of a set of parameters passed.  |
| [piol_set_sort](#exseis-piol-piol_set_sort) | Sort the set by the specified sort type.  |
| [piol_set_sort_fn](#exseis-piol-piol_set_sort_fn) | Sort the set using a custom comparison function.  |
| [piol_set_taper](#exseis-piol-piol_set_taper) | Perform tailed taper on a set of traces.  |
| [piol_set_output](#exseis-piol-piol_set_output) | Output using the given output prefix.  |
| [piol_set_text](#exseis-piol-piol_set_text) | Set the text-header of the output.  |
| [piol_set_summary](#exseis-piol-piol_set_summary) | Summarise the current status by whatever means the PIOL instrinsically supports.  |
| [piol_set_add](#exseis-piol-piol_set_add) | Add a file to the set based on the name given.  |
| [piol_set_agc](#exseis-piol-piol_set_agc) | Scale traces using automatic gain control for visualization.  |


## C API
| Name | Description | 
| ---- | ---- |
| [piol_exseis](#exseis-piol-piol_exseis) | This class provides access to the ExSeisPIOL class but with a simpler API.  |
| [piol_file_rule](#exseis-piol-piol_file_rule) | Rules associated with trace parameters.  |
| [piol_file_trace_metadata](#exseis-piol-piol_file_trace_metadata) | Class for initialising the trace parameter structure and storing a structure with the necessary rules.  |
| [piol_file_read_interface](#exseis-piol-piol_file_read_interface) | The `Input_file` class is a generic interface for reading seismic data files.  |
| [piol_file_write_interface](#exseis-piol-piol_file_write_interface) | The `Output_file` class is a generic interface for writing seismic data files.  |


## PIOL calls. Non-file specific
| Name | Description | 
| ---- | ---- |
| [piol_exseis_new](#exseis-piol-piol_exseis_new) | Initialise the PIOL and MPI.  |
| [piol_exseis_delete](#exseis-piol-piol_exseis_delete) | close the PIOL (deinit MPI)  |
| [piol_exseis_get_rank](#exseis-piol-piol_exseis_get_rank) | Get the rank of the process (in terms of the PIOL communicator)  |
| [piol_exseis_get_num_rank](#exseis-piol-piol_exseis_get_num_rank) | Get the number of processes (in terms of the PIOL communicator)  |
| [piol_exseis_assert_ok](#exseis-piol-piol_exseis_assert_ok) | Check if the PIOL has any error conditions.  |
| [piol_exseis_barrier](#exseis-piol-piol_exseis_barrier) | A barrier. All PIOL processes must call this.  |
| [piol_exseis_max](#exseis-piol-piol_exseis_max) | Return the maximum value amongst the processes.  |


## SEG-Y Size functions
| Name | Description | 
| ---- | ---- |
| [piol_segy_segy_text_header_size](#exseis-piol-piol_segy_segy_text_header_size) | Get the size of the SEG-Y text field (3200 bytes)  |
| [piol_segy_get_file_size](#exseis-piol-piol_segy_get_file_size) | Get the size a SEGY file should be given the number of traces (`nt`) and sample size (`ns`)  |
| [piol_segy_segy_trace_data_size](#exseis-piol-piol_segy_segy_trace_data_size) | Get the size a SEGY trace should be given the sample size (`ns`) and a type of float.  |
| [piol_segy_segy_trace_header_size](#exseis-piol-piol_segy_segy_trace_header_size) | Get the size of a SEGY trace header.  |


## Rule calls
| Name | Description | 
| ---- | ---- |
| [piol_file_rule_new](#exseis-piol-piol_file_rule_new) | Initialise a Rule structure.  |
| [piol_file_rule_new_from_list](#exseis-piol-piol_file_rule_new_from_list) | Initialise a Rule structure from a list of Metas.  |
| [piol_file_rule_delete](#exseis-piol-piol_file_rule_delete) | Free a Rule structure.  |
| [piol_file_rule_add_rule_meta](#exseis-piol-piol_file_rule_add_rule_meta) | Add a pre-defined rule.  |
| [piol_file_rule_add_rule_rule](#exseis-piol-piol_file_rule_add_rule_rule) | Add all rules from the given handle.  |
| [piol_file_rule_add_long](#exseis-piol-piol_file_rule_add_long) | Add a Rule for longs (int64_t)  |
| [piol_file_rule_add_short](#exseis-piol-piol_file_rule_add_short) | Add a Rule for shorts (int16_t)  |
| [piol_file_rule_add_segy_float](#exseis-piol-piol_file_rule_add_segy_float) | Add a Rule for floats.  |
| [piol_file_rule_add_index](#exseis-piol-piol_file_rule_add_index) | Add a rule for an index.  |
| [piol_file_rule_add_copy](#exseis-piol-piol_file_rule_add_copy) | Add a rule to buffer the original trace header.  |
| [piol_file_rule_rm_rule](#exseis-piol-piol_file_rule_rm_rule) | Remove a rule for a parameter.  |
| [piol_file_rule_extent](#exseis-piol-piol_file_rule_extent) | Return the size of the buffer space required for the metadata items when converting to SEG-Y.  |
| [piol_file_rule_memory_usage](#exseis-piol-piol_file_rule_memory_usage) | Estimate of the total memory used.  |
| [piol_file_rule_memory_usage_per_header](#exseis-piol-piol_file_rule_memory_usage_per_header) | How much memory will each set of parameters require?  |


## Trace_metadata calls
| Name | Description | 
| ---- | ---- |
| [piol_file_trace_metadata_new](#exseis-piol-piol_file_trace_metadata_new) | Define a new parameter structure.  |
| [piol_file_trace_metadata_delete](#exseis-piol-piol_file_trace_metadata_delete) | Free the given parameter structure.  |
| [piol_file_trace_metadata_size](#exseis-piol-piol_file_trace_metadata_size) | Return the number of sets of trace parameters.  |
| [piol_file_trace_metadata_memory_usage](#exseis-piol-piol_file_trace_metadata_memory_usage) | Estimate of the total memory used.  |
| [piol_file_get_prm_index](#exseis-piol-piol_file_get_prm_index) | Get an index parameter which is in a particular set in a parameter structure.  |
| [piol_file_get_prm_integer](#exseis-piol-piol_file_get_prm_integer) | Get a long parameter which is in a particular set in a parameter structure.  |
| [piol_file_get_prm_double](#exseis-piol-piol_file_get_prm_double) | Get a double parameter which is in a particular set in a parameter structure.  |
| [piol_file_set_prm_index](#exseis-piol-piol_file_set_prm_index) | Set a index parameter within the parameter structure.  |
| [piol_file_set_prm_integer](#exseis-piol-piol_file_set_prm_integer) | Set a long parameter within the parameter structure.  |
| [piol_file_set_prm_double](#exseis-piol-piol_file_set_prm_double) | Set a double parameter within the parameter structure.  |
| [piol_file_cpy_prm](#exseis-piol-piol_file_cpy_prm) | Copy parameter within the parameter structure.  |


## Operations
| Name | Description | 
| ---- | ---- |
| [piol_file_get_min_max](#exseis-piol-piol_file_get_min_max) | Find the traces with the min and max of a supplied set of coordinates within a file.  |


## Opening and closing files
| Name | Description | 
| ---- | ---- |
| [piol_file_read_segy_new](#exseis-piol-piol_file_read_segy_new) | Open a read-only file and return a handle for the file.  |
| [piol_file_write_segy_new](#exseis-piol-piol_file_write_segy_new) | Open a write-only file and return a handle for the file.  |
| [piol_file_read_interface_delete](#exseis-piol-piol_file_read_interface_delete) | Close the file associated with the handle.  |
| [piol_file_write_interface_delete](#exseis-piol-piol_file_write_interface_delete) | Close the file associated with the handle.  |


## Read binary and text headers
| Name | Description | 
| ---- | ---- |
| [piol_file_read_interface_read_text](#exseis-piol-piol_file_read_interface_read_text) | Read the human readable text from the file.  |
| [piol_file_read_interface_read_ns](#exseis-piol-piol_file_read_interface_read_ns) | Read the number of samples per trace.  |
| [piol_file_read_interface_read_nt](#exseis-piol-piol_file_read_interface_read_nt) | Read the number of traces in the file.  |
| [piol_file_read_interface_read_sample_interval](#exseis-piol-piol_file_read_interface_read_sample_interval) | Read the increment between trace samples.  |
| [piol_file_write_interface_write_text](#exseis-piol-piol_file_write_interface_write_text) | Write the human readable text from the file.  |
| [piol_file_write_interface_write_ns](#exseis-piol-piol_file_write_interface_write_ns) | Write the number of samples per trace.  |
| [piol_file_write_interface_write_nt](#exseis-piol-piol_file_write_interface_write_nt) | Write the number of traces in the file.  |
| [piol_file_write_interface_write_sample_interval](#exseis-piol-piol_file_write_interface_write_sample_interval) | Write the increment between trace samples.  |


## Reading/writing data from the trace headers
| Name | Description | 
| ---- | ---- |
| [piol_file_write_interface_write_param](#exseis-piol-piol_file_write_interface_write_param) | Write the trace parameters from offset to offset+sz to the respective trace headers.  |
| [piol_file_read_interface_read_param](#exseis-piol-piol_file_read_interface_read_param) | Write the trace parameters from offset to offset+sz to the respective trace headers.  |


## Reading the traces themselves
| Name | Description | 
| ---- | ---- |
| [piol_file_read_interface_read_trace](#exseis-piol-piol_file_read_interface_read_trace) | Read the traces and trace parameters from offset to offset+sz.  |
| [piol_file_write_interface_write_trace](#exseis-piol-piol_file_write_interface_write_trace) | Read the traces and trace parameters from offset to offset+sz.  |


## Lists
| Name | Description | 
| ---- | ---- |
| [piol_file_read_interface_read_trace_non_contiguous](#exseis-piol-piol_file_read_interface_read_trace_non_contiguous) | Read the traces and trace parameters corresponding to the list of trace numbers.  |
| [piol_file_read_interface_read_trace_non_monotonic](#exseis-piol-piol_file_read_interface_read_trace_non_monotonic) | Read the traces and trace parameters corresponding to the non-monotonic list of trace numbers.  |
| [piol_file_write_interface_write_trace_non_contiguous](#exseis-piol-piol_file_write_interface_write_trace_non_contiguous) | Write the traces corresponding to the list of trace numbers.  |
| [piol_file_write_interface_write_param_non_contiguous](#exseis-piol-piol_file_write_interface_write_param_non_contiguous) | Write the trace parameters corresponding to the list of trace numbers.  |
| [piol_file_read_interface_read_param_non_contiguous](#exseis-piol-piol_file_read_interface_read_param_non_contiguous) | Read the trace parameters corresponding to the list of trace numbers.  |



## PIOL Set functions
### <a name='exseis-piol-piol_set_new' /> public PIOL_Set * exseis::piol::piol_set_new (const piol_exseis *piol, const char *pattern)

Initialise the set. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | The PIOL handle  |
| in | const char * | pattern | The file-matching pattern  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| PIOL_Set * | The set handle  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_delete' /> public void exseis::piol::piol_set_delete (PIOL_Set *set)

Free (deinit) the set. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_get_min_max' /> public void exseis::piol::piol_set_get_min_max (PIOL_Set *set, exseis_Trace_metadata_key m1, exseis_Trace_metadata_key m2, struct PIOL_CoordElem *minmax)

Get the min and the max of a set of parameters passed. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | exseis_Trace_metadata_key | m1 | The first parameter type  |
| in | exseis_Trace_metadata_key | m2 | The second parameter type  |
| out | struct PIOL_CoordElem * | minmax | An array of structures containing the minimum item.x, maximum item.x, minimum item.y, maximum item.y and their respective trace numbers.  |







This is a parallel operation. It is the collective min and max across all processes (which also must all call this file). 




[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_sort' /> public void exseis::piol::piol_set_sort (PIOL_Set *set, exseis_SortType type)

Sort the set by the specified sort type. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | exseis_SortType | type | The sort type  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_sort_fn' /> public void exseis::piol::piol_set_sort_fn (PIOL_Set *set, bool(*func)(const piol_file_trace_metadata *param, size_t i, size_t j))

Sort the set using a custom comparison function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | A handle for the set.  |
| in | bool(*)(const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] *param, size_t i, size_t j) | func | The custom comparison function to sort set  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_taper' /> public void exseis::piol::piol_set_taper (PIOL_Set *set, exseis_Taper_function taper_function, size_t ntpstr, size_t ntpend)

Perform tailed taper on a set of traces. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | A handle for the set  |
| in | exseis_Taper_function | taper_function | The type of taper to be applied to traces.  |
| in | size_t | ntpstr | The length of left-tail taper ramp.  |
| in | size_t | ntpend | The length of right-tail taper ramp (pass 0 for no ramp).  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_output' /> public void exseis::piol::piol_set_output (PIOL_Set *set, const char *oname)

Output using the given output prefix. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | const char * | oname | The output prefix  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_text' /> public void exseis::piol::piol_set_text (PIOL_Set *set, const char *outmsg)

Set the text-header of the output. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | const char * | outmsg | The output message  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_summary' /> public void exseis::piol::piol_set_summary (const PIOL_Set *set)

Summarise the current status by whatever means the PIOL instrinsically supports. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const PIOL_Set * | set | The set handle  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_add' /> public void exseis::piol::piol_set_add (PIOL_Set *set, const char *name)

Add a file to the set based on the name given. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | const char * | name | The input name  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_set_agc' /> public void exseis::piol::piol_set_agc (PIOL_Set *set, exseis_Gain_function type, size_t window, exseis_Trace_value target_amplitude)

Scale traces using automatic gain control for visualization. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | PIOL_Set * | set | The set handle  |
| in | exseis_Gain_function | type | They type of agc scaling function used  |
| in | size_t | window | Length of the agc window  |
| in | exseis_Trace_value | target_amplitude | Normalization value  |












[Go to Top](#exseis-piol)

## C API
### <a name='exseis-piol-piol_exseis' /> public exseis::piol::piol_exseis 

This class provides access to the ExSeisPIOL class but with a simpler API. 








[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule' /> public exseis::piol::piol_file_rule 

Rules associated with trace parameters. 










The structure which holds the rules associated with the trace parameters in a file. These rules describe how to interpret the metadata and also how to index the parameter structure of arrays. 




[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_trace_metadata' /> public exseis::piol::piol_file_trace_metadata 

Class for initialising the trace parameter structure and storing a structure with the necessary rules. 








[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface' /> public exseis::piol::piol_file_read_interface 

The `Input_file` class is a generic interface for reading seismic data files. 








[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface' /> public exseis::piol::piol_file_write_interface 

The `Output_file` class is a generic interface for writing seismic data files. 








[Go to Top](#exseis-piol)

## PIOL calls. Non-file specific
### <a name='exseis-piol-piol_exseis_new' /> public [piol_exseis][exseis-piol-piol_exseis] * exseis::piol::piol_exseis_new (exseis_Verbosity verbosity)

Initialise the PIOL and MPI. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [exseis_Verbosity][exseis-utils-logging-exseis_Verbosity] | verbosity | The level of verbosity to log. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_exseis][exseis-piol-piol_exseis] * | A handle to the PIOL. |












> **pre:** `verbosity` is in the enum `exseis::utils::Verbosity` 




[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_delete' /> public void exseis::piol::piol_exseis_delete (piol_exseis *piol)

close the PIOL (deinit MPI) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_get_rank' /> public size_t exseis::piol::piol_exseis_get_rank (const piol_exseis *piol)

Get the rank of the process (in terms of the PIOL communicator) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The rank of the current process.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_get_num_rank' /> public size_t exseis::piol::piol_exseis_get_num_rank (const piol_exseis *piol)

Get the number of processes (in terms of the PIOL communicator) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of ranks the `piol` is running on.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_assert_ok' /> public void exseis::piol::piol_exseis_assert_ok (const piol_exseis *piol, const char *msg)

Check if the PIOL has any error conditions. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |
| in | const char * | msg | The message to print to the log.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_barrier' /> public void exseis::piol::piol_exseis_barrier (const piol_exseis *piol)

A barrier. All PIOL processes must call this. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_exseis_max' /> public size_t exseis::piol::piol_exseis_max (const piol_exseis *piol, size_t n)

Return the maximum value amongst the processes. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |
| in | size_t | n | The value to take part in the reduction |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the maximum value of (`n`) amongst the processes  |












[Go to Top](#exseis-piol)

## SEG-Y Size functions
### <a name='exseis-piol-piol_segy_segy_text_header_size' /> public size_t exseis::piol::piol_segy_segy_text_header_size (void)

Get the size of the SEG-Y text field (3200 bytes) 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The text size in bytes for SEG-Y  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_segy_get_file_size' /> public size_t exseis::piol::piol_segy_get_file_size (size_t nt, size_t ns)

Get the size a SEGY file should be given the number of traces (`nt`) and sample size (`ns`) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | The number of traces  |
| in | size_t | ns | The number of samples per trace |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The corresponding file size in bytes for SEG-Y  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_segy_segy_trace_data_size' /> public size_t exseis::piol::piol_segy_segy_trace_data_size (size_t ns)

Get the size a SEGY trace should be given the sample size (`ns`) and a type of float. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | ns | The number of samples per trace |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The corresponding trace size in bytes  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_segy_segy_trace_header_size' /> public size_t exseis::piol::piol_segy_segy_trace_header_size (void)

Get the size of a SEGY trace header. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The trace header size in bytes  |












[Go to Top](#exseis-piol)

## Rule calls
### <a name='exseis-piol-piol_file_rule_new' /> public [piol_file_rule][exseis-piol-piol_file_rule] * exseis::piol::piol_file_rule_new (bool def)

Initialise a Rule structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | bool | def | Set default rules if true |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_file_rule][exseis-piol-piol_file_rule] * | Return a handle for the Rule structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_new_from_list' /> public [piol_file_rule][exseis-piol-piol_file_rule] * exseis::piol::piol_file_rule_new_from_list (const exseis_Trace_metadata_key *m, size_t n)

Initialise a Rule structure from a list of Metas. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const exseis_Trace_metadata_key * | m | List of Meta values (size n).  |
| in | size_t | n | Number of elements in m |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_file_rule][exseis-piol-piol_file_rule] * | Return a handle for the rule structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_delete' /> public void exseis::piol::piol_file_rule_delete (piol_file_rule *rule)

Free a Rule structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle associated with the structure to free  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_rule_meta' /> public bool exseis::piol::piol_file_rule_add_rule_meta (piol_file_rule *rule, exseis_Trace_metadata_key m)

Add a pre-defined rule. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | exseis_Trace_metadata_key | m | The Meta entry. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Return true if the rule was added, otherwise false  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_rule_rule' /> public bool exseis::piol::piol_file_rule_add_rule_rule (piol_file_rule *rule, const piol_file_rule *rule_to_copy)

Add all rules from the given handle. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | const [piol_file_rule][exseis-piol-piol_file_rule] * | rule_to_copy | The rule handle to copy the rules from. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Return true if no errors  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_long' /> public void exseis::piol::piol_file_rule_add_long (piol_file_rule *rule, exseis_Trace_metadata_key m, exseis_Trace_header_offsets loc)

Add a Rule for longs (int64_t) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | exseis_Trace_metadata_key | m | The parameter which one is providing a rule for  |
| in | exseis_Trace_header_offsets | loc | The location in the trace header for the rule.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_short' /> public void exseis::piol::piol_file_rule_add_short (piol_file_rule *rule, exseis_Trace_metadata_key m, exseis_Trace_header_offsets loc)

Add a Rule for shorts (int16_t) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | exseis_Trace_metadata_key | m | The parameter which one is providing a rule for  |
| in | exseis_Trace_header_offsets | loc | The location in the trace header for the rule.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_segy_float' /> public void exseis::piol::piol_file_rule_add_segy_float (piol_file_rule *rule, exseis_Trace_metadata_key m, exseis_Trace_header_offsets loc, exseis_Trace_header_offsets scalar_location)

Add a Rule for floats. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | exseis_Trace_metadata_key | m | The parameter which one is providing a rule for  |
| in | exseis_Trace_header_offsets | loc | The location in the trace header for the rule.  |
| in | exseis_Trace_header_offsets | scalar_location | The location in the trace header for the shared scaler;  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_index' /> public void exseis::piol::piol_file_rule_add_index (piol_file_rule *rule, exseis_Trace_metadata_key m)

Add a rule for an index. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |
| in | exseis_Trace_metadata_key | m | The Meta entry.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_add_copy' /> public void exseis::piol::piol_file_rule_add_copy (piol_file_rule *rule)

Add a rule to buffer the original trace header. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_rm_rule' /> public void exseis::piol::piol_file_rule_rm_rule (piol_file_rule *rule, exseis_Trace_metadata_key m)

Remove a rule for a parameter. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle associated with the structure to free  |
| in | exseis_Trace_metadata_key | m | The parameter which one is removing a rule for  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_extent' /> public size_t exseis::piol::piol_file_rule_extent (piol_file_rule *rule)

Return the size of the buffer space required for the metadata items when converting to SEG-Y. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return the size.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_memory_usage' /> public size_t exseis::piol::piol_file_rule_memory_usage (const piol_file_rule *rule)

Estimate of the total memory used. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return estimate in bytes.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_rule_memory_usage_per_header' /> public size_t exseis::piol::piol_file_rule_memory_usage_per_header (const piol_file_rule *rule)

How much memory will each set of parameters require? 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Amount of memory in bytes.  |












[Go to Top](#exseis-piol)

## Trace_metadata calls
### <a name='exseis-piol-piol_file_trace_metadata_new' /> public [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * exseis::piol::piol_file_trace_metadata_new (piol_file_rule *rule, size_t sz)

Define a new parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_rule][exseis-piol-piol_file_rule] * | rule | The Rule handle associated with the structure (use NULL for default rules) A copy of this pointer is stored in piol_file_trace_metadata, but it's safe to call piol_file_rule_delete(rule) before deleting this, because the underlying type is a shared_ptr.  |
| in | size_t | sz | The number of sets of parameters stored by the structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | Return a handle for the parameter structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_trace_metadata_delete' /> public void exseis::piol::piol_file_trace_metadata_delete (piol_file_trace_metadata *param)

Free the given parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_trace_metadata_size' /> public size_t exseis::piol::piol_file_trace_metadata_size (const piol_file_trace_metadata *param)

Return the number of sets of trace parameters. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Number of sets  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_trace_metadata_memory_usage' /> public size_t exseis::piol::piol_file_trace_metadata_memory_usage (const piol_file_trace_metadata *param)

Estimate of the total memory used. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Return estimate in bytes.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_get_prm_index' /> public size_t exseis::piol::piol_file_get_prm_index (size_t i, exseis_Trace_metadata_key entry, const piol_file_trace_metadata *param)

Get an index parameter which is in a particular set in a parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The associated parameter  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_get_prm_integer' /> public exseis_Integer exseis::piol::piol_file_get_prm_integer (size_t i, exseis_Trace_metadata_key entry, const piol_file_trace_metadata *param)

Get a long parameter which is in a particular set in a parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis_Integer | The associated parameter  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_get_prm_double' /> public exseis_Floating_point exseis::piol::piol_file_get_prm_double (size_t i, exseis_Trace_metadata_key entry, const piol_file_trace_metadata *param)

Get a double parameter which is in a particular set in a parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis_Floating_point | The associated parameter  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_set_prm_index' /> public void exseis::piol::piol_file_set_prm_index (size_t i, exseis_Trace_metadata_key entry, size_t ret, piol_file_trace_metadata *param)

Set a index parameter within the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | size_t | ret | The value to set the parameter to  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_set_prm_integer' /> public void exseis::piol::piol_file_set_prm_integer (size_t i, exseis_Trace_metadata_key entry, exseis_Integer ret, piol_file_trace_metadata *param)

Set a long parameter within the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | exseis_Integer | ret | The value to set the parameter to  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_set_prm_double' /> public void exseis::piol::piol_file_set_prm_double (size_t i, exseis_Trace_metadata_key entry, exseis_Floating_point ret, piol_file_trace_metadata *param)

Set a double parameter within the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number  |
| in | exseis_Trace_metadata_key | entry | The parameter entry  |
| in | exseis_Floating_point | ret | The value to set the parameter to  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | The parameter structure  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_cpy_prm' /> public void exseis::piol::piol_file_cpy_prm (size_t i, const piol_file_trace_metadata *src, size_t j, piol_file_trace_metadata *dst)

Copy parameter within the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The parameter set number of the source  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | src | The parameter structure of the source  |
| in | size_t | j | The parameter set number of the destination  |
| inout | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | dst | The parameter structure of the destination  |












[Go to Top](#exseis-piol)

## Operations
### <a name='exseis-piol-piol_file_get_min_max' /> public void exseis::piol::piol_file_get_min_max (const piol_exseis *piol, size_t offset, size_t sz, exseis_Trace_metadata_key m1, exseis_Trace_metadata_key m2, const piol_file_trace_metadata *param, struct PIOL_CoordElem *minmax)

Find the traces with the min and max of a supplied set of coordinates within a file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of local traces to process.  |
| in | exseis_Trace_metadata_key | m1 | The first coordinate item of interest.  |
| in | exseis_Trace_metadata_key | m2 | The second coordinate item of interest.  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure.  |
| out | struct PIOL_CoordElem * | minmax | Set `minmax` to structs corresponding to the minimum x, maximum x, minimum y, maximum y in that order.  |












[Go to Top](#exseis-piol)

## Opening and closing files
### <a name='exseis-piol-piol_file_read_segy_new' /> public [piol_file_read_interface][exseis-piol-piol_file_read_interface] * exseis::piol::piol_file_read_segy_new (const piol_exseis *piol, const char *name)

Open a read-only file and return a handle for the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |
| in | const char * | name | The name of the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | A handle for the file.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_segy_new' /> public [piol_file_write_interface][exseis-piol-piol_file_write_interface] * exseis::piol::piol_file_write_segy_new (const piol_exseis *piol, const char *name)

Open a write-only file and return a handle for the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_exseis][exseis-piol-piol_exseis] * | piol | A handle to the PIOL.  |
| in | const char * | name | The name of the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | A handle for the file.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_delete' /> public void exseis::piol::piol_file_read_interface_delete (piol_file_read_interface *read_direct)

Close the file associated with the handle. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_delete' /> public void exseis::piol::piol_file_write_interface_delete (piol_file_write_interface *write_direct)

Close the file associated with the handle. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| inout | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |












[Go to Top](#exseis-piol)

## Read binary and text headers
### <a name='exseis-piol-piol_file_read_interface_read_text' /> public const char * exseis::piol::piol_file_read_interface_read_text (const piol_file_read_interface *read_direct)

Read the human readable text from the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| const char * | A string containing the text (in ASCII format)  |







When read_text is called the ExSeisPIOL is responsible for the memory returned. The string should not be dereferenced after the associated file is closed.




[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_ns' /> public size_t exseis::piol::piol_file_read_interface_read_ns (const piol_file_read_interface *read_direct)

Read the number of samples per trace. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of samples per trace  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_nt' /> public size_t exseis::piol::piol_file_read_interface_read_nt (const piol_file_read_interface *read_direct)

Read the number of traces in the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | The number of traces  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_sample_interval' /> public double exseis::piol::piol_file_read_interface_read_sample_interval (const piol_file_read_interface *read_direct)

Read the increment between trace samples. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| double | The increment between trace samples  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_text' /> public void exseis::piol::piol_file_write_interface_write_text (piol_file_write_interface *write_direct, const char *text)

Write the human readable text from the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | const char * | text | The new null-terminated string containing the text (in ASCII format).  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_ns' /> public void exseis::piol::piol_file_write_interface_write_ns (piol_file_write_interface *write_direct, size_t ns)

Write the number of samples per trace. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | ns | The new number of samples per trace.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_nt' /> public void exseis::piol::piol_file_write_interface_write_nt (piol_file_write_interface *write_direct, size_t nt)

Write the number of traces in the file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | nt | The new number of traces.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_sample_interval' /> public void exseis::piol::piol_file_write_interface_write_sample_interval (piol_file_write_interface *write_direct, exseis_Floating_point sample_interval)

Write the increment between trace samples. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | exseis_Floating_point | sample_interval | The new interval between trace samples.  |












[Go to Top](#exseis-piol)

## Reading/writing data from the trace headers
### <a name='exseis-piol-piol_file_write_interface_write_param' /> public void exseis::piol::piol_file_write_interface_write_param (piol_file_write_interface *write_direct, size_t offset, size_t sz, const piol_file_trace_metadata *param)

Write the trace parameters from offset to offset+sz to the respective trace headers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure. |








It is assumed that this operation is not an update. Any previous contents of the trace header will be overwritten. 




[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_param' /> public void exseis::piol::piol_file_read_interface_read_param (const piol_file_read_interface *read_direct, size_t offset, size_t sz, piol_file_trace_metadata *param)

Write the trace parameters from offset to offset+sz to the respective trace headers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure.  |












[Go to Top](#exseis-piol)

## Reading the traces themselves
### <a name='exseis-piol-piol_file_read_interface_read_trace' /> public void exseis::piol::piol_file_read_interface_read_trace (const piol_file_read_interface *read_direct, size_t offset, size_t sz, exseis_Trace_value *trace, piol_file_trace_metadata *param)

Read the traces and trace parameters from offset to offset+sz. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process.  |
| out | exseis_Trace_value * | trace | A contiguous array of each trace (size sz*ns).  |
| out | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_trace' /> public void exseis::piol::piol_file_write_interface_write_trace (piol_file_write_interface *write_direct, size_t offset, size_t sz, exseis_Trace_value *trace, const piol_file_trace_metadata *param)

Read the traces and trace parameters from offset to offset+sz. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | offset | The starting trace number.  |
| in | size_t | sz | The number of traces to process  |
| out | exseis_Trace_value * | trace | A contiguous array of each trace (size sz*ns).  |
| in | const [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure. |









> **warning:** This function is not thread safe. 




[Go to Top](#exseis-piol)

## Lists
### <a name='exseis-piol-piol_file_read_interface_read_trace_non_contiguous' /> public void exseis::piol::piol_file_read_interface_read_trace_non_contiguous (piol_file_read_interface *read_direct, size_t sz, const size_t *offset, exseis_Trace_value *trace, piol_file_trace_metadata *param)

Read the traces and trace parameters corresponding to the list of trace numbers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |
| in | size_t | sz | The number of traces to process.  |
| in | const size_t * | offset | A list of trace numbers (size sz).  |
| out | exseis_Trace_value * | trace | A contiguous array of each trace (size sz*ns).  |
| out | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure (pass NULL to ignore).  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_trace_non_monotonic' /> public void exseis::piol::piol_file_read_interface_read_trace_non_monotonic (piol_file_read_interface *read_direct, size_t sz, const size_t *offset, exseis_Trace_value *trace, piol_file_trace_metadata *param)

Read the traces and trace parameters corresponding to the non-monotonic list of trace numbers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |
| in | size_t | sz | The number of traces to process.  |
| in | const size_t * | offset | A non-monotonic list of trace numbers (size sz).  |
| out | exseis_Trace_value * | trace | A contiguous array of each trace (size sz*ns).  |
| out | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle for the parameter structure (pass NULL to ignore).  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_trace_non_contiguous' /> public void exseis::piol::piol_file_write_interface_write_trace_non_contiguous (piol_file_write_interface *write_direct, size_t sz, const size_t *offset, exseis_Trace_value *trace, piol_file_trace_metadata *param)

Write the traces corresponding to the list of trace numbers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | sz | The number of traces to process.  |
| in | const size_t * | offset | A list of trace numbers (size sz).  |
| in | exseis_Trace_value * | trace | A contiguous array of each trace (size sz*ns).  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | A handle to the parameter structure (pass NULL to ignore).  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_write_interface_write_param_non_contiguous' /> public void exseis::piol::piol_file_write_interface_write_param_non_contiguous (piol_file_write_interface *write_direct, size_t sz, const size_t *offset, piol_file_trace_metadata *param)

Write the trace parameters corresponding to the list of trace numbers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_write_interface][exseis-piol-piol_file_write_interface] * | write_direct | A handle for the file.  |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | A list of trace numbers (size sz).  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | An handle to the parameter structure.  |












[Go to Top](#exseis-piol)

### <a name='exseis-piol-piol_file_read_interface_read_param_non_contiguous' /> public void exseis::piol::piol_file_read_interface_read_param_non_contiguous (piol_file_read_interface *read_direct, size_t sz, const size_t *offset, piol_file_trace_metadata *param)

Read the trace parameters corresponding to the list of trace numbers. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [piol_file_read_interface][exseis-piol-piol_file_read_interface] * | read_direct | A handle for the file.  |
| in | size_t | sz | The number of traces to process  |
| in | const size_t * | offset | A list of trace numbers (size sz).  |
| in | [piol_file_trace_metadata][exseis-piol-piol_file_trace_metadata] * | param | An handle to the parameter structure.  |












[Go to Top](#exseis-piol)

[exseis-piol-piol_exseis]:./index.md#exseis-piol-piol_exseis
[exseis-piol-piol_file_read_interface]:./index.md#exseis-piol-piol_file_read_interface
[exseis-piol-piol_file_rule]:./index.md#exseis-piol-piol_file_rule
[exseis-piol-piol_file_trace_metadata]:./index.md#exseis-piol-piol_file_trace_metadata
[exseis-piol-piol_file_write_interface]:./index.md#exseis-piol-piol_file_write_interface
[exseis-utils-logging-exseis_Verbosity]:./../utils/logging/index.md#exseis-utils-logging-exseis_Verbosity
