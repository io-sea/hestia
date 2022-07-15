# <a name='exseis-utils-logging-Source_position' /> public exseis::utils::logging::Source_position

A representation of a point in the source code. 



This is intended to be used for logging the position a [Log_entry][exseis-utils-logging-Log_entry] was created.

The macro EXSEISDAT_SOURCE_POSITION is provided to construct this from the function name with the file and line number information added automatically. 




## Public Attributes
| Name | Description | 
| ---- | ---- |
| [function_name](#exseis-utils-logging-Source_position-function_name) | The name of the function containing the given point in the code.  |
| [file_name](#exseis-utils-logging-Source_position-file_name) | The name of the file containing the given point in the code.  |
| [line_number](#exseis-utils-logging-Source_position-line_number) | The line number in the file containing the given point in the code.  |



## Public Attributes
### <a name='exseis-utils-logging-Source_position-function_name' /> public exseis::utils::logging::Source_position::function_name 

The name of the function containing the given point in the code. 








[Go to Top](#exseis-utils-logging-Source_position)

### <a name='exseis-utils-logging-Source_position-file_name' /> public exseis::utils::logging::Source_position::file_name 

The name of the file containing the given point in the code. 








[Go to Top](#exseis-utils-logging-Source_position)

### <a name='exseis-utils-logging-Source_position-line_number' /> public exseis::utils::logging::Source_position::line_number 

The line number in the file containing the given point in the code. 








[Go to Top](#exseis-utils-logging-Source_position)

[exseis-utils-logging-Log_entry]:./Log_entry.md#exseis-utils-logging-Log_entry
