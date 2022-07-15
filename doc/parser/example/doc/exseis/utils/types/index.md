# <a name='exseis-utils-types' />  exseis::utils::types

Aliases for fundamental types and conversions used throughout the ExSeisDat project. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Native_from_type](./Native_from_type.md) | Get the native data type from the [Type][exseis-utils-types-Type].  |
| [Type_from_native](./Type_from_native.md) | Get the [Type][exseis-utils-types-Type] of a native data type.  |


## Namespaces
| Name | Description | 
| ---- | ---- |
| [mpi](./mpi/index.md) | Aliases for MPI datatypes.  |


## C_API
| Name | Description | 
| ---- | ---- |
| [exseis_Integer](#exseis-utils-types-exseis_Integer) | C API for [exseis::utils::types::Integer][exseis-utils-types-Integer].  |
| [exseis_Floating_point](#exseis-utils-types-exseis_Floating_point) | C API for [exseis::utils::types::Floating_point][exseis-utils-types-Floating_point].  |
| [exseis_Trace_value](#exseis-utils-types-exseis_Trace_value) | C API for [exseis::utils::types::Trace_value][exseis-utils-types-Trace_value].  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [Type](#exseis-utils-types-Type) | An enumeration of all the arithmetic types used by ExSeisDat.  |


## Type Definitions
| Name | Description | 
| ---- | ---- |
| [Integer](#exseis-utils-types-Integer) | A fundamental signed integer type.  |
| [Floating_point](#exseis-utils-types-Floating_point) | A fundamental signed floating-point type.  |
| [Trace_value](#exseis-utils-types-Trace_value) | The floating-point type used to store trace values.  |



## C_API
### <a name='exseis-utils-types-exseis_Integer' /> public exseis::utils::types::exseis_Integer 

C API for [exseis::utils::types::Integer][exseis-utils-types-Integer]. 










A fundamental signed integer type. This is used where fundamental integer type is needed, large enough to hold whatever value we might need it to, without explicitly specifying the size. 




[Go to Top](#exseis-utils-types)

### <a name='exseis-utils-types-exseis_Floating_point' /> public exseis::utils::types::exseis_Floating_point 

C API for [exseis::utils::types::Floating_point][exseis-utils-types-Floating_point]. 










A fundamental signed floating-point type. This is used where fundamental floating-point type is needed, large enough to hold whatever value we might need it to, without explicitly specifying the size. 




[Go to Top](#exseis-utils-types)

### <a name='exseis-utils-types-exseis_Trace_value' /> public exseis::utils::types::exseis_Trace_value 

C API for [exseis::utils::types::Trace_value][exseis-utils-types-Trace_value]. 










The floating-point type used to store trace values. Traces values are usually stored in a lossy, compressed form. As a result, we don't need a high-precision storage type. 




[Go to Top](#exseis-utils-types)

## Enumerations
### <a name='exseis-utils-types-Type' /> public exseis::utils::types::Type

An enumeration of all the arithmetic types used by ExSeisDat. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Double | Represents `double`  |  |
| Float | Represents `float`  |  |
| Int64 | Represents `int64_t`  |  |
| UInt64 | Represents `uint64_t`  |  |
| Int32 | Represents `int32_t`  |  |
| UInt32 | Represents `uint32_t`  |  |
| Int16 | Represents `int16_t`  |  |
| UInt16 | Represents `uint16_t`  |  |
| Int8 | Represents `int8_t`  |  |
| UInt8 | Represents `uint8_t`  |  |
| Index | Temporary! Should be removed.  |  |
| Copy | Temporary! Should be removed.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-utils-types)

## Type Definitions
### <a name='exseis-utils-types-Integer' /> public exseis::utils::types::Integer 

A fundamental signed integer type. 










This is used where fundamental integer type is needed, large enough to hold whatever value we might need it to, without explicitly specifying the size. 




[Go to Top](#exseis-utils-types)

### <a name='exseis-utils-types-Floating_point' /> public exseis::utils::types::Floating_point 

A fundamental signed floating-point type. 










This is used where fundamental floating-point type is needed, large enough to hold whatever value we might need it to, without explicitly specifying the size. 




[Go to Top](#exseis-utils-types)

### <a name='exseis-utils-types-Trace_value' /> public exseis::utils::types::Trace_value 

The floating-point type used to store trace values. 










Traces values are usually stored in a lossy, compressed form. As a result, we don't need a high-precision storage type. 




[Go to Top](#exseis-utils-types)

[exseis-utils-types-Floating_point]:./index.md#exseis-utils-types-Floating_point
[exseis-utils-types-Integer]:./index.md#exseis-utils-types-Integer
[exseis-utils-types-Trace_value]:./index.md#exseis-utils-types-Trace_value
[exseis-utils-types-Type]:./index.md#exseis-utils-types-Type
