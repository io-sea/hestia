# <a name='exseis-flow-Set' /> public exseis::flow::Set

The internal set class. 




## Non-Core
| Name | Description | 
| ---- | ---- |
| [sort](#exseis-flow-Set-sort) | Sort the set by the specified sort type.  |
| [get_min_max](#exseis-flow-Set-get_min_max) | Get the min and the max of a set of parameters passed.  |
| [temporal_filter](#exseis-flow-Set-temporal_filter) | Filter traces or part of traces using a IIR Butterworth filter.  |
| [temporal_filter](#exseis-flow-Set-temporal_filter-1) | Filter traces or part of traces using a IIR Butterworth filter.  |
| [temporal_filter](#exseis-flow-Set-temporal_filter-2) | Filter traces or part of traces using a IIR Butterworth filter.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [FileDeque](#exseis-flow-Set-FileDeque) | Typedef for passing in a list of FileDesc objects.  |
| [FuncLst](#exseis-flow-Set-FuncLst) | The function list type for the set layer.  |


## Protected Attributes
| Name | Description | 
| ---- | ---- |
| [m_piol](#exseis-flow-Set-m_piol) | The PIOL object.  |
| [m_outfix](#exseis-flow-Set-m_outfix) | The output prefix.  |
| [m_outmsg](#exseis-flow-Set-m_outmsg) | The output text-header message.  |
| [m_file](#exseis-flow-Set-m_file) | A deque of unique pointers to file descriptors.  |
| [m_fmap](#exseis-flow-Set-m_fmap) |  |
| [m_offmap](#exseis-flow-Set-m_offmap) | A map of (ns, sample_interval) key to the current offset.  |
| [m_rule](#exseis-flow-Set-m_rule) | Contains a pointer to the Rules for parameters.  |
| [m_cache](#exseis-flow-Set-m_cache) | The cache of parameters and traces.  |
| [m_func](#exseis-flow-Set-m_func) | The list of functions and related data.  |
| [m_rank](#exseis-flow-Set-m_rank) | The rank of the particular process.  |
| [m_num_rank](#exseis-flow-Set-m_num_rank) | The number of ranks.  |


## Protected Functions
| Name | Description | 
| ---- | ---- |
| [drop](#exseis-flow-Set-drop) | Drop all file descriptors without output.  |
| [start_subset](#exseis-flow-Set-start_subset) | Start unwinding the function list for subset-only operations based on the given iterators.  |
| [start_gather](#exseis-flow-Set-start_gather) | Start unwinding the function list for gather operations on the given iterators.  |
| [start_single](#exseis-flow-Set-start_single) | Start unwinding the function list for single-trace operations on the given iterators.  |
| [calc_func](#exseis-flow-Set-calc_func) | The entry point for unwinding the function list for all use-cases.  |
| [calc_func](#exseis-flow-Set-calc_func-1) | The entry point for unwinding the function list for single-traces and gathers only.  |
| [calc_func_s](#exseis-flow-Set-calc_func_s) | The entry point for unwinding the function list for subsets.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Set](#exseis-flow-Set-Set) | Constructor.  |
| [Set](#exseis-flow-Set-Set-1) | Constructor.  |
| [Set](#exseis-flow-Set-Set-2) | Constructor overload.  |
| [~Set](#exseis-flow-Set-~Set) | Destructor.  |
| [sort](#exseis-flow-Set-sort-1) | Sort the set using the given comparison function.  |
| [sort](#exseis-flow-Set-sort-2) | Sort the set using the given comparison function.  |
| [output](#exseis-flow-Set-output) | Output using the given output prefix.  |
| [output](#exseis-flow-Set-output-1) | Output using the output prefix stored as member variable.  |
| [get_min_max](#exseis-flow-Set-get_min_max-1) | Find the min and max of two given parameters (e.g x and y source coordinates) and return the associated values and trace numbers int the given structure.  |
| [taper](#exseis-flow-Set-taper) | Function to add to modify function that applies a 2 tailed taper to a set of traces.  |
| [mute](#exseis-flow-Set-mute) | Function to modify function that applies both a mute region at the start and end of a series of traces and a 2 tailed taper.  |
| [agc](#exseis-flow-Set-agc) | Function to add to modify function that applies automatic gain control to a set of traces.  |
| [text](#exseis-flow-Set-text) | [Set][exseis-flow-Set] the text-header of the output.  |
| [summary](#exseis-flow-Set-summary) | Summarise the current status by whatever means the PIOL intrinsically supports.  |
| [add](#exseis-flow-Set-add) | Add a file to the set based on the ReadInterface.  |
| [add](#exseis-flow-Set-add-1) | Add a file to the set based on the pattern/name given.  |
| [to_angle](#exseis-flow-Set-to_angle) | Perform the radon to angle conversion. This assumes the input set is a radon transform file.  |



## Non-Core
### <a name='exseis-flow-Set-sort' /> public void exseis::flow::Set::sort (exseis::piol::Sort_type type)

Sort the set by the specified sort type. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::piol::Sort_type | type | The sort type  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-get_min_max' /> public void exseis::flow::Set::get_min_max (exseis::piol::Trace_metadata_key m1, exseis::piol::Trace_metadata_key m2, exseis::piol::CoordElem *minmax)

Get the min and the max of a set of parameters passed. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::piol::Trace_metadata_key | m1 | The first parameter type  |
| in | exseis::piol::Trace_metadata_key | m2 | The second parameter type  |
| out | exseis::piol::CoordElem * | minmax | An array of structures containing the minimum item.x, maximum item.x, minimum item.y, maximum item.y and their respective trace numbers.  |







This is a parallel operation. It is the collective min and max across all processes (which also must all call this file).




[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-temporal_filter' /> public void exseis::flow::Set::temporal_filter (exseis::utils::FltrType type, exseis::utils::FltrDmn domain, exseis::utils::PadType pad, exseis::utils::Trace_value fs, std::vector< exseis::utils::Trace_value > corners, size_t nw=0U, size_t win_cntr=0U)

Filter traces or part of traces using a IIR Butterworth filter. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::FltrType | type | Type of filter (i.e. lowpass, highpass, bandpass)  |
| in | exseis::utils::FltrDmn | domain | Filtering domaini  |
| in | exseis::utils::PadType | pad | Padding pattern  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | std::vector< exseis::utils::Trace_value > | corners | Passband and stopband frequency in Hz  |
| in | size_t | nw | Size of trace filtering window  |
| in | size_t | win_cntr | Center of trace filtering window  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-temporal_filter-1' /> public void exseis::flow::Set::temporal_filter (exseis::utils::FltrType type, exseis::utils::FltrDmn domain, exseis::utils::PadType pad, exseis::utils::Trace_value fs, size_t n, std::vector< exseis::utils::Trace_value > corners, size_t nw=0U, size_t win_cntr=0U)

Filter traces or part of traces using a IIR Butterworth filter. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::FltrType | type | Type of filter (i.e. lowpass, highpass, bandpass)  |
| in | exseis::utils::FltrDmn | domain | Filtering domain  |
| in | exseis::utils::PadType | pad | Padding pattern  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | std::vector< exseis::utils::Trace_value > | corners | Passband frequency in Hz  |
| in | size_t | n | Filter order  |
| in | size_t | nw | Size of trace filtering window  |
| in | size_t | win_cntr | Center of trace filtering window  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-temporal_filter-2' /> public void exseis::flow::Set::temporal_filter (exseis::utils::FltrType type, exseis::utils::FltrDmn domain, exseis::utils::PadType pad, exseis::utils::Trace_value fs, size_t n, exseis::utils::Trace_value corners, size_t nw=0U, size_t win_cntr=0U)

Filter traces or part of traces using a IIR Butterworth filter. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::FltrType | type | Type of filter (i.e. lowpass, highpass, bandpass)  |
| in | exseis::utils::FltrDmn | domain | Filtering domain  |
| in | exseis::utils::PadType | pad | Padding pattern  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | exseis::utils::Trace_value | corners | Passband frequency in Hz  |
| in | size_t | n | Filter order  |
| in | size_t | nw | Size of trace filtering window  |
| in | size_t | win_cntr | Center of trace filtering window  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-flow-Set)

## Public Types
### <a name='exseis-flow-Set-FileDeque' /> public exseis::flow::Set::FileDeque 

Typedef for passing in a list of FileDesc objects. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-FuncLst' /> public exseis::flow::Set::FuncLst 

The function list type for the set layer. 








[Go to Top](#exseis-flow-Set)

## Protected Attributes
### <a name='exseis-flow-Set-m_piol' /> protected exseis::flow::Set::m_piol 

The PIOL object. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_outfix' /> protected exseis::flow::Set::m_outfix 

The output prefix. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_outmsg' /> protected exseis::flow::Set::m_outmsg 

The output text-header message. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_file' /> protected exseis::flow::Set::m_file 

A deque of unique pointers to file descriptors. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_fmap' /> protected exseis::flow::Set::m_fmap 










A map of (ns, sample_interval) key to a deque of file descriptor pointers 




[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_offmap' /> protected exseis::flow::Set::m_offmap 

A map of (ns, sample_interval) key to the current offset. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_rule' /> protected exseis::flow::Set::m_rule 

Contains a pointer to the Rules for parameters. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_cache' /> protected exseis::flow::Set::m_cache 

The cache of parameters and traces. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_func' /> protected exseis::flow::Set::m_func 

The list of functions and related data. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_rank' /> protected exseis::flow::Set::m_rank 

The rank of the particular process. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-m_num_rank' /> protected exseis::flow::Set::m_num_rank 

The number of ranks. 








[Go to Top](#exseis-flow-Set)

## Protected Functions
### <a name='exseis-flow-Set-drop' /> protected void exseis::flow::Set::drop ()

Drop all file descriptors without output. 








#### Qualifiers: 
* inline


[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-start_subset' /> protected FuncLst::iterator exseis::flow::Set::start_subset (FuncLst::iterator f_curr, FuncLst::iterator f_end)

Start unwinding the function list for subset-only operations based on the given iterators. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| FuncLst::iterator | Return the final iterator reached by the last deque.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-start_gather' /> protected std::string exseis::flow::Set::start_gather (FuncLst::iterator f_curr, FuncLst::iterator f_end)

Start unwinding the function list for gather operations on the given iterators. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::string | Return a file name if f_end is reached. Otherwise return "".  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-start_single' /> protected std::vector< std::string > exseis::flow::Set::start_single (FuncLst::iterator f_curr, FuncLst::iterator f_end)

Start unwinding the function list for single-trace operations on the given iterators. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< std::string > | Return a list of all file names. Files are currently always created.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-calc_func' /> protected std::vector< std::string > exseis::flow::Set::calc_func (FuncLst::iterator f_curr, FuncLst::iterator f_end)

The entry point for unwinding the function list for all use-cases. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< std::string > | Return a list of all file names.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-calc_func-1' /> protected std::unique_ptr< detail::Trace_block > exseis::flow::Set::calc_func (FuncLst::iterator f_curr, FuncLst::iterator f_end, detail::Function_options type, std::unique_ptr< detail::Trace_block > b_in)

The entry point for unwinding the function list for single-traces and gathers only. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |
| in | detail::Function_options | type | The type of function currently being processed. Either single trace or gather.  |
| in | std::unique_ptr< detail::Trace_block > | b_in | The input trace block which can contain traces and trace parameters.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< detail::Trace_block > | Return a traceblock which contains the output from the operation. |











Transitions from gather to single trace are allowed but not the inverse. 




[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-calc_func_s' /> protected FuncLst::iterator exseis::flow::Set::calc_func_s (FuncLst::iterator f_curr, FuncLst::iterator f_end, FileDeque &f_que)

The entry point for unwinding the function list for subsets. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | FuncLst::iterator | f_curr | The iterator for the current function to process.  |
| in | FuncLst::iterator | f_end | The iterator which indicates the end of the list has been reached.  |
| in | [FileDeque][exseis-flow-Set-FileDeque] & | f_que | A deque of unique pointers to file descriptors.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| FuncLst::iterator | Return the final iterator reached.  |












[Go to Top](#exseis-flow-Set)

## Public Functions
### <a name='exseis-flow-Set-Set' /> public  exseis::flow::Set::Set (std::shared_ptr< exseis::piol::ExSeisPIOL > piol, std::string pattern, std::string outfix, std::shared_ptr< exseis::piol::Rule > rule=std::make_shared< exseis::piol::Rule >(std::initializer_list< exseis::piol::Trace_metadata_key >{ piol::Trace_metadata_key::Copy}))

Constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< exseis::piol::ExSeisPIOL > | piol | The PIOL object.  |
| in | std::string | pattern | The file-matching pattern  |
| in | std::string | outfix | The output file-name prefix  |
| in | std::shared_ptr< exseis::piol::Rule > | rule | Contains a pointer to the rules to use for trace parameters.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-Set-1' /> public  exseis::flow::Set::Set (std::shared_ptr< exseis::piol::ExSeisPIOL > piol, std::string pattern, std::shared_ptr< exseis::piol::Rule > rule=std::make_shared< exseis::piol::Rule >(std::initializer_list< exseis::piol::Trace_metadata_key >{ piol::Trace_metadata_key::Copy}))

Constructor. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< exseis::piol::ExSeisPIOL > | piol | The PIOL object.  |
| in | std::string | pattern | The file-matching pattern  |
| in | std::shared_ptr< exseis::piol::Rule > | rule | Contains a pointer to the rules to use for trace parameters.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-Set-2' /> public  exseis::flow::Set::Set (std::shared_ptr< exseis::piol::ExSeisPIOL > piol, std::shared_ptr< exseis::piol::Rule > rule=std::make_shared< exseis::piol::Rule >(std::initializer_list< exseis::piol::Trace_metadata_key >{ piol::Trace_metadata_key::Copy}))

Constructor overload. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< exseis::piol::ExSeisPIOL > | piol | The PIOL object.  |
| in | std::shared_ptr< exseis::piol::Rule > | rule | Contains a pointer to the rules to use for trace parameters.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-~Set' /> public  exseis::flow::Set::~Set ()

Destructor. 








[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-sort-1' /> public void exseis::flow::Set::sort (exseis::piol::CompareP sort_func)

Sort the set using the given comparison function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::piol::CompareP | sort_func | The comparison function  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-sort-2' /> public void exseis::flow::Set::sort (std::shared_ptr< exseis::piol::Rule > r, exseis::piol::CompareP sort_func)

Sort the set using the given comparison function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::shared_ptr< exseis::piol::Rule > | r | The rules necessary for the sort.  |
| in | exseis::piol::CompareP | sort_func | The comparison function.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-output' /> public std::vector< std::string > exseis::flow::Set::output (std::string oname)

Output using the given output prefix. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | oname | The output prefix  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::vector< std::string > | Return a vector of the actual output names.  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-output-1' /> public void exseis::flow::Set::output ()

Output using the output prefix stored as member variable. 















[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-get_min_max-1' /> public void exseis::flow::Set::get_min_max (exseis::piol::MinMaxFunc< exseis::piol::Trace_metadata > xlam, exseis::piol::MinMaxFunc< exseis::piol::Trace_metadata > ylam, exseis::piol::CoordElem *minmax)

Find the min and max of two given parameters (e.g x and y source coordinates) and return the associated values and trace numbers int the given structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::piol::MinMaxFunc< exseis::piol::Trace_metadata > | xlam | The function for returning the first parameter  |
| in | exseis::piol::MinMaxFunc< exseis::piol::Trace_metadata > | ylam | The function for returning the second parameter  |
| out | exseis::piol::CoordElem * | minmax | The array of structures to hold the ouput  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-taper' /> public void exseis::flow::Set::taper (exseis::utils::Taper_function taper_function, size_t taper_size_at_begin, size_t taper_size_at_end=0)

Function to add to modify function that applies a 2 tailed taper to a set of traces. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Taper_function | taper_function | Weight function for the taper ramp  |
| in | size_t | taper_size_at_begin | Length of taper at beginning of trace  |
| in | size_t | taper_size_at_end | Length of taper at end of trace  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-mute' /> public void exseis::flow::Set::mute (exseis::utils::Taper_function taper_function, size_t mute_size_at_begin, size_t taper_size_at_begin, size_t taper_size_at_end, size_t mute_size_at_end)

Function to modify function that applies both a mute region at the start and end of a series of traces and a 2 tailed taper. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Taper_function | taper_function | Weight function for the taper ramp  |
| in | size_t | taper_size_at_begin | Length of taper at beginning of trace  |
| in | size_t | taper_size_at_end | Length of taper at end of trace  |
| in | size_t | mute_size_at_begin | Length of mute before taper at beginning of trace  |
| in | size_t | mute_size_at_end | Length of the mute at end of trace  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-agc' /> public void exseis::flow::Set::agc (exseis::utils::Gain_function agc_func, size_t window, exseis::utils::Trace_value target_amplitude)

Function to add to modify function that applies automatic gain control to a set of traces. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Gain_function | agc_func | Staistical function used to scale traces  |
| in | size_t | window | Length of the agc window  |
| in | exseis::utils::Trace_value | target_amplitude | Value to which traces are normalized  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-text' /> public void exseis::flow::Set::text (std::string outmsg)

[Set][exseis-flow-Set] the text-header of the output. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | outmsg | The output message  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-summary' /> public void exseis::flow::Set::summary () const

Summarise the current status by whatever means the PIOL intrinsically supports. 








#### Qualifiers: 
* const


[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-add' /> public void exseis::flow::Set::add (std::unique_ptr< exseis::piol::Input_file > in)

Add a file to the set based on the ReadInterface. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::unique_ptr< exseis::piol::Input_file > | in | The file interface  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-add-1' /> public void exseis::flow::Set::add (std::string name)

Add a file to the set based on the pattern/name given. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | name | The input name or pattern  |












[Go to Top](#exseis-flow-Set)

### <a name='exseis-flow-Set-to_angle' /> public void exseis::flow::Set::to_angle (std::string vm_name, size_t v_bin, size_t output_traces_per_gather, exseis::utils::Floating_point output_sample_interval=std::atan(1) *4.0/180)

Perform the radon to angle conversion. This assumes the input set is a radon transform file. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::string | vm_name | The name of the velocity model file.  |
| in | size_t | v_bin | The velocity model bin value.  |
| in | size_t | output_traces_per_gather | The number of traces in the output gather.  |
| in | exseis::utils::Floating_point | output_sample_interval | The samples per trace for the output in radians (i.e the angle increment between samples.) (default = 1 degree).  |












[Go to Top](#exseis-flow-Set)

[exseis-flow-Set]:./Set.md
[exseis-flow-Set-FileDeque]:./Set.md#exseis-flow-Set-FileDeque
[exseis-flow-Set-FuncLst]:./Set.md#exseis-flow-Set-FuncLst
