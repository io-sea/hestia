# <a name='exseis-piol-segy' />  exseis::piol::segy

SEGY specific parameters and functions. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Segy_file_header_byte](./Segy_file_header_byte.md) | The global file headers for the SEGY format.  |


## C API
| Name | Description | 
| ---- | ---- |
| [@2](#exseis-piol-segy-@2) | C API for [exseis::piol::segy::Trace_header_offsets][exseis-piol-segy-Trace_header_offsets].  |
| [exseis_Trace_header_offsets](#exseis-piol-segy-exseis_Trace_header_offsets) | C API for [exseis::piol::segy::Trace_header_offsets][exseis-piol-segy-Trace_header_offsets].  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [Trace_header_offsets](#exseis-piol-segy-Trace_header_offsets) | SEG-Y Trace Header offsets.  |
| [Segy_number_format](#exseis-piol-segy-Segy_number_format) | The Number Format, identifying the representation used for numbers in the SEGY file.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [extract_trace_metadata](#exseis-piol-segy-extract_trace_metadata) | Extract parameters from an unsigned char array into the parameter structure.  |
| [insert_trace_metadata](#exseis-piol-segy-insert_trace_metadata) | Extract parameters from an unsigned char array into the parameter structure.  |
| [parse_scalar](#exseis-piol-segy-parse_scalar) | Convert a SEG-Y scale integer to a floating point type.  |
| [find_scalar](#exseis-piol-segy-find_scalar) | Take a coordinate and extract a suitable scale factor to represent that number in 6 byte fixed point format of the SEG-Y specification.  |
| [segy_text_header_size](#exseis-piol-segy-segy_text_header_size) | Return the size of the text field.  |
| [segy_binary_file_header_size](#exseis-piol-segy-segy_binary_file_header_size) | Return the size of the Binary File Header in bytes.  |
| [segy_trace_header_size](#exseis-piol-segy-segy_trace_header_size) | Return the size of the Standard Trace Header in bytes.  |
| [segy_trace_data_size](#exseis-piol-segy-segy_trace_data_size) | Return the size of the Trace Data.  |
| [segy_trace_size](#exseis-piol-segy-segy_trace_size) | Return the size of the Trace, i.e. the Trace Metadata + Trace Data.  |
| [get_file_size](#exseis-piol-segy-get_file_size) | Return the expected size of the file if there are nt data-objects and ns elements in a data-field.  |
| [segy_trace_location](#exseis-piol-segy-segy_trace_location) | Return the offset location of a specific data object.  |
| [segy_trace_data_location](#exseis-piol-segy-segy_trace_data_location) | Return the offset location of a specific data-field.  |
| [get_nt](#exseis-piol-segy-get_nt) | Return the number of traces in a file given a file size.  |



## C API
### <a name='exseis-piol-segy-@2' /> public exseis::piol::segy::@2

C API for [exseis::piol::segy::Trace_header_offsets][exseis-piol-segy-Trace_header_offsets]. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| exseis_tr_SeqNum | int32_t. The trace sequence number in the Line.  | = 1U |
| exseis_tr_SeqFNum | int32_t. The trace sequence number in SEG-Y File.  | = 5U |
| exseis_tr_ORF | int32_t. The original field record number.  | = 9U |
| exseis_tr_TORF | int32_t. The trace number in the ORF.  | = 13U |
| exseis_tr_ENSrcNum | int32_t. The source energy number.  | = 17U |
| exseis_tr_SeqNumEns | int32_t. The trace number in the ensemble.  | = 25U |
| exseis_tr_TIC | int16_t. The trace identification number.  | = 29U |
| exseis_tr_VStackCnt | int16_t. The number of traces vertically stacked.  | = 31U |
| exseis_tr_HStackCnt | int16_t. The number of traces horizontally stacked.  | = 33U |
| exseis_tr_CDist | int32_t. The distance from source center to receiver centre.  | = 37U |
| exseis_tr_RcvElv | int32_t. The receiver group elevation.  | = 41U |
| exseis_tr_SurfElvSrc | int32_t. The surface elevation at the source.  | = 45U |
| exseis_tr_SrcDpthSurf | int32_t. The source depth below surface (opposite of above?).  | = 49U |
| exseis_tr_DtmElvRcv | int32_t. The datum elevation for the receiver group.  | = 53U |
| exseis_tr_DtmElvSrc | int32_t. The datum elevation for the source.  | = 57U |
| exseis_tr_WtrDepSrc | int32_t. The water depth for the source.  | = 61U |
| exseis_tr_WtrDepRcv | int32_t. The water depth for the receive group.  | = 65U |
| exseis_tr_ScaleElev | int16_t. The scale coordinate for 41-68 (elevations + depths).  | = 69U |
| exseis_tr_ScaleCoord | int16_t. The scale coordinate for 73-88 + 181-188  | = 71U |
| exseis_tr_x_src | int32_t. The X coordinate for the source  | = 73U |
| exseis_tr_y_src | int32_t. The Y coordinate for the source  | = 77U |
| exseis_tr_x_rcv | int32_t. The X coordinate for the receive group  | = 81U |
| exseis_tr_y_rcv | int32_t. The Y coordinate for the receive group  | = 85U |
| exseis_tr_UpSrc | int16_t. The uphole time at the source (ms).  | = 95U |
| exseis_tr_UpRcv | int16_t. The uphole time at the receive group (ms).  | = 97U |
| exseis_tr_Ns | int16_t. The number of samples in the trace.  | = 115U |
| exseis_tr_sample_interval | int16_t. The sample interval (us).  | = 117U |
| exseis_tr_xCmp | int32_t. The X coordinate for the CMP  | = 181U |
| exseis_tr_yCmp | int32_t. The Y coordinate for the CMP  | = 185U |
| exseis_tr_il | int32_t. The Inline grid point.  | = 189U |
| exseis_tr_xl | int32_t. The Crossline grid point.  | = 193U |
| exseis_tr_ShotNum | int32_t. The source nearest to the CDP.  | = 197U |
| exseis_tr_ShotScal | int16_t. The shot number scalar. (Explicitly says that 0 == 1)  | = 201U |
| exseis_tr_ValMeas | int16_t. The unit system used for trace values.  | = 203U |
| exseis_tr_TransConst | int32_t. The transduction constant.  | = 205U |
| exseis_tr_TransExp | int16_t. The transduction exponent.  | = 209U |
| exseis_tr_TransUnit | int16_t. The transduction units  | = 211U |
| exseis_tr_TimeScal | int16_t. Scalar for time measurements.  | = 215U |
| exseis_tr_SrcMeas | int32_t. Source measurement.  | = 225U |
| exseis_tr_SrcMeasExp | int16_t. Source measurement exponent.  | = 229U |





SEG-Y Trace Header offsets.




[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-exseis_Trace_header_offsets' /> public exseis::piol::segy::exseis_Trace_header_offsets 

C API for [exseis::piol::segy::Trace_header_offsets][exseis-piol-segy-Trace_header_offsets]. 










SEG-Y Trace Header offsets.




[Go to Top](#exseis-piol-segy)

## Enumerations
### <a name='exseis-piol-segy-Trace_header_offsets' /> public exseis::piol::segy::Trace_header_offsets

SEG-Y Trace Header offsets. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| SeqNum | int32_t. The trace sequence number in the Line.  | = 1U |
| SeqFNum | int32_t. The trace sequence number in SEG-Y File.  | = 5U |
| ORF | int32_t. The original field record number.  | = 9U |
| TORF | int32_t. The trace number in the ORF.  | = 13U |
| ENSrcNum | int32_t. The source energy number.  | = 17U |
| SeqNumEns | int32_t. The trace number in the ensemble.  | = 25U |
| TIC | int16_t. The trace identification number.  | = 29U |
| VStackCnt | int16_t. The number of traces vertically stacked.  | = 31U |
| HStackCnt | int16_t. The number of traces horizontally stacked.  | = 33U |
| CDist | int32_t. The distance from source center to receiver centre.  | = 37U |
| RcvElv | int32_t. The receiver group elevation.  | = 41U |
| SurfElvSrc | int32_t. The surface elevation at the source.  | = 45U |
| SrcDpthSurf | int32_t. The source depth below surface (opposite of above?).  | = 49U |
| DtmElvRcv | int32_t. The datum elevation for the receiver group.  | = 53U |
| DtmElvSrc | int32_t. The datum elevation for the source.  | = 57U |
| WtrDepSrc | int32_t. The water depth for the source.  | = 61U |
| WtrDepRcv | int32_t. The water depth for the receive group.  | = 65U |
| ScaleElev | int16_t. The scale coordinate for 41-68 (elevations + depths).  | = 69U |
| ScaleCoord | int16_t. The scale coordinate for 73-88 + 181-188  | = 71U |
| x_src | int32_t. The X coordinate for the source  | = 73U |
| y_src | int32_t. The Y coordinate for the source  | = 77U |
| x_rcv | int32_t. The X coordinate for the receive group  | = 81U |
| y_rcv | int32_t. The Y coordinate for the receive group  | = 85U |
| UpSrc | int16_t. The uphole time at the source (ms).  | = 95U |
| UpRcv | int16_t. The uphole time at the receive group (ms).  | = 97U |
| Ns | int16_t. The number of samples in the trace.  | = 115U |
| sample_interval | int16_t. The sample interval (us).  | = 117U |
| xCmp | int32_t. The X coordinate for the CMP  | = 181U |
| yCmp | int32_t. The Y coordinate for the CMP  | = 185U |
| il | int32_t. The Inline grid point.  | = 189U |
| xl | int32_t. The Crossline grid point.  | = 193U |
| ShotNum | int32_t. The source nearest to the CDP.  | = 197U |
| ShotScal | int16_t. The shot number scalar. (Explicitly says that 0 == 1)  | = 201U |
| ValMeas | int16_t. The unit system used for trace values.  | = 203U |
| TransConst | int32_t. The transduction constant.  | = 205U |
| TransExp | int16_t. The transduction exponent.  | = 209U |
| TransUnit | int16_t. The transduction units  | = 211U |
| TimeScal | int16_t. Scalar for time measurements.  | = 215U |
| SrcMeas | int32_t. Source measurement.  | = 225U |
| SrcMeasExp | int16_t. Source measurement exponent.  | = 229U |



#### Qualifiers: 
* strong


[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-Segy_number_format' /> public exseis::piol::segy::Segy_number_format

The Number Format, identifying the representation used for numbers in the SEGY file. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| IBM | IBM format, big endian.  | = 1 |
| TC4 | Two's complement, 4 byte.  | = 2 |
| TC2 | Two's complement, 2 byte.  | = 3 |
| FPG | Fixed-point gain (obsolete)  | = 4 |
| IEEE | The IEEE format, big endian.  | = 5 |
| NA1 | Unused.  | = 6 |
| NA2 | Unused.  | = 7 |
| TC1 | Two's complement, 1 byte.  | = 8 |



#### Qualifiers: 
* strong


[Go to Top](#exseis-piol-segy)

## Functions
### <a name='exseis-piol-segy-extract_trace_metadata' /> public void exseis::piol::segy::extract_trace_metadata (size_t sz, const unsigned char *md, Trace_metadata &prm, size_t stride, size_t skip)

Extract parameters from an unsigned char array into the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of sets of parameters  |
| in | const unsigned char * | md | The buffer in the SEG-Y trace header format  |
| out | Trace_metadata & | prm | The parameter structure  |
| in | size_t | stride | The stride to use between adjacent blocks in the input buffer.  |
| in | size_t | skip | Skip the first "skip" entries when filling Trace_metadata  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-insert_trace_metadata' /> public void exseis::piol::segy::insert_trace_metadata (size_t sz, const Trace_metadata &prm, unsigned char *md, size_t stride, size_t skip)

Extract parameters from an unsigned char array into the parameter structure. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | sz | The number of sets of parameters  |
| in | const Trace_metadata & | prm | The parameter structure  |
| out | unsigned char * | md | The buffer in the SEG-Y trace header format  |
| in | size_t | stride | The stride to use between adjacent blocks in the input buffer.  |
| in | size_t | skip | Skip the first "skip" entries when extracting entries from Trace_metadata  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-parse_scalar' /> public exseis::utils::Floating_point exseis::piol::segy::parse_scalar (int16_t segy_scalar)

Convert a SEG-Y scale integer to a floating point type. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int16_t | segy_scalar | The int16_t scale taken from the SEG-Y file |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Floating_point | The scale convertered to floating point.  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-find_scalar' /> public int16_t exseis::piol::segy::find_scalar (exseis::utils::Floating_point val)

Take a coordinate and extract a suitable scale factor to represent that number in 6 byte fixed point format of the SEG-Y specification. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Floating_point | val | The coordinate of interest. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int16_t | An appropriate scale factor for the coordinate. |











Convert the number from float to a 6 byte SEGY fixed-point representation. There are ten possible values for the scale factor. Shown are the possible values and the form the input float should have to use that scale factor. firstly, anything smaller than 4 decimal points is discarded since the approach can not represent it.

Shown is the position of the least significant digit, `d:` 
```c++
-10000 - d0000.0000
-1000  - d000.0000
-100   - d00.0000
-10    - d0.0000
-1     - d
1      - d
10     - d.d
100    - d.dd
1000   - d.ddd
10000  - d.dddd
```

> **[Todo][todo]:** Handle the annoying case of numbers at or around 2147483648 with a decimal somewhere. 

Add rounding before positive scale values 




[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_text_header_size' /> public constexpr size_t exseis::piol::segy::segy_text_header_size ()

Return the size of the text field. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| constexpr size_t | Returns the size of the text field in bytes  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_binary_file_header_size' /> public constexpr size_t exseis::piol::segy::segy_binary_file_header_size ()

Return the size of the Binary File Header in bytes. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| constexpr size_t | Returns the size of the HO in bytes.  |







This assumes text extensions are not used




[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_trace_header_size' /> public constexpr size_t exseis::piol::segy::segy_trace_header_size ()

Return the size of the Standard Trace Header in bytes. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| constexpr size_t | Returns the size of the Trace Metadata in bytes  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_trace_data_size' /> public constexpr size_t exseis::piol::segy::segy_trace_data_size (size_t ns)

Return the size of the Trace Data. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| constexpr size_t | Returns the size of the data-field in bytes  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_trace_size' /> public size_t exseis::piol::segy::segy_trace_size (size_t ns)

Return the size of the Trace, i.e. the Trace Metadata + Trace Data. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The datatype of the data-field. The default value is float. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Returns the Trace size.  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-get_file_size' /> public size_t exseis::piol::segy::get_file_size (size_t nt, size_t ns)

Return the expected size of the file if there are nt data-objects and ns elements in a data-field. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The datatype of the data-field. The default value is float. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | The number of data objects.  |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Returns the expected file size.  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_trace_location' /> public size_t exseis::piol::segy::segy_trace_location (size_t i, size_t ns)

Return the offset location of a specific data object. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The datatype of the data-field. The default value is float. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The location of the ith data object will be returned.  |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Returns the location.  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-segy_trace_data_location' /> public size_t exseis::piol::segy::segy_trace_data_location (size_t i, size_t ns)

Return the offset location of a specific data-field. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The datatype of the data-field. The default value is float. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | i | The location of the ith data-field will be returned.  |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Returns the location.  |












[Go to Top](#exseis-piol-segy)

### <a name='exseis-piol-segy-get_nt' /> public size_t exseis::piol::segy::get_nt (size_t fsz, size_t ns)

Return the number of traces in a file given a file size. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The datatype of the data-field. The default value is float. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | fsz | the size of a file or expected size in bytes  |
| in | size_t | ns | The number of elements in the data-field. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Returns the number of traces.  |












[Go to Top](#exseis-piol-segy)

[exseis-piol-segy-Trace_header_offsets]:./index.md#exseis-piol-segy-Trace_header_offsets
[todo]:./../../../todo.md#todo
