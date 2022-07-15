# <a name='exseis-piol-segy-Segy_file_header_byte' /> public exseis::piol::segy::Segy_file_header_byte

The global file headers for the SEGY format. 



This represents the supported global headers for the SEGY format. The values used are the byte offset into the file for the given parameter, as defined by the SEGY standard. The standard itself, however, presents them 1-indexed, so we write them here in the 1-indexed form, and subtract 1 for a c-style 0-indexed offset. 




## Public Static Attributes
| Name | Description | 
| ---- | ---- |
| [interval](#exseis-piol-segy-Segy_file_header_byte-interval) | int16_t. The increment between traces in microseconds  |
| [num_sample](#exseis-piol-segy-Segy_file_header_byte-num_sample) | int16_t. The number of samples per trace  |
| [type](#exseis-piol-segy-Segy_file_header_byte-type) | int16_t. Trace data type. AKA format in SEGY terminology  |
| [sort](#exseis-piol-segy-Segy_file_header_byte-sort) | int16_t. The sort order of the traces.  |
| [units](#exseis-piol-segy-Segy_file_header_byte-units) | int16_t. The unit system, i.e SI or imperial.  |
| [segy_format](#exseis-piol-segy-Segy_file_header_byte-segy_format) | int16_t. The SEG-Y Revision number  |
| [fixed_trace](#exseis-piol-segy-Segy_file_header_byte-fixed_trace) | int16_t. Whether we are using fixed traces or not.  |
| [extensions](#exseis-piol-segy-Segy_file_header_byte-extensions) | int16_t. If we use header extensions or not.  |



## Public Static Attributes
### <a name='exseis-piol-segy-Segy_file_header_byte-interval' /> public exseis::piol::segy::Segy_file_header_byte::interval  = 3217U - 1U

int16_t. The increment between traces in microseconds 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-num_sample' /> public exseis::piol::segy::Segy_file_header_byte::num_sample  = 3221U - 1U

int16_t. The number of samples per trace 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-type' /> public exseis::piol::segy::Segy_file_header_byte::type  = 3225U - 1U

int16_t. Trace data type. AKA format in SEGY terminology 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-sort' /> public exseis::piol::segy::Segy_file_header_byte::sort  = 3229U - 1U

int16_t. The sort order of the traces. 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-units' /> public exseis::piol::segy::Segy_file_header_byte::units  = 3255U - 1U

int16_t. The unit system, i.e SI or imperial. 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-segy_format' /> public exseis::piol::segy::Segy_file_header_byte::segy_format  = 3501U - 1U

int16_t. The SEG-Y Revision number 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-fixed_trace' /> public exseis::piol::segy::Segy_file_header_byte::fixed_trace  = 3503U - 1U

int16_t. Whether we are using fixed traces or not. 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

### <a name='exseis-piol-segy-Segy_file_header_byte-extensions' /> public exseis::piol::segy::Segy_file_header_byte::extensions  = 3505U - 1U

int16_t. If we use header extensions or not. 








#### Qualifiers: 
* static


[Go to Top](#exseis-piol-segy-Segy_file_header_byte)

