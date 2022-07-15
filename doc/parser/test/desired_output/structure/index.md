# <a name='structure' />  structure

basic structural tests to check, that all documented elements appear in the documentation 




## Classes
| Name | Description | 
| ---- | ---- |
| [class](./class.md) | class for testing class structures  |
| [class_with_template](./class_with_template.md) | a class with a templated type  |
| [struct](./struct.md) | a struct  |


## Namespaces
| Name | Description | 
| ---- | ---- |
| [function_only_namespace](./function_only_namespace/index.md) | this namespace will contain only functions, as this created problems with doxygen's default html output  |
| [inner_namespace](./inner_namespace/index.md) | nested namespace to test nested structures - who would have thought?!?  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [normal_enum](#structure-normal_enum) | normal enum  |
| [class_enum](#structure-class_enum) | class enum  |
| [class_enum_inheritance](#structure-class_enum_inheritance) | class enum with inheritance  |
| [@0](#structure-@0) | unnamed enum  |


## Type Definitions
| Name | Description | 
| ---- | ---- |
| [this_is_a_int](#structure-this_is_a_int) | typedefs inside a namespace  |


## Variables
| Name | Description | 
| ---- | ---- |
| [public_variable](#structure-public_variable) | public variable  |
| [protected_variable](#structure-protected_variable) | protected variable  |
| [private_variable](#structure-private_variable) | private variable  |
| [private_variable_with_value](#structure-private_variable_with_value) | variable with default value  |


## Functions
| Name | Description | 
| ---- | ---- |
| [function_without_parameters](#structure-function_without_parameters) | function without anything  |
| [function_set_to_default](#structure-function_set_to_default) | function with initializer  |
| [function_with_one_argument](#structure-function_with_one_argument) | function with one argument  |
| [function_with_in_argument](#structure-function_with_in_argument) | function with one "in" argument  |
| [function_with_default_value_argument](#structure-function_with_default_value_argument) | function with one "in" argument, that has a default value  |
| [function_with_out_argument](#structure-function_with_out_argument) | function with one "out" argument  |
| [function_with_two_arguments](#structure-function_with_two_arguments) | function with two arguments  |
| [function_with_return_value](#structure-function_with_return_value) | function with return value  |
| [const_function](#structure-const_function) | a const function - this is hopefully similar to override and such  |
| [function_with_template](#structure-function_with_template) | function with template  |
| [function_with_exception](#structure-function_with_exception) | function that throws an exception  |
| [function_with_retval](#structure-function_with_retval) | function that strange retval command  |



## Enumerations
### <a name='structure-normal_enum' /> public structure::normal_enum

normal enum 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| one | Enum value 1.  |  |
| two | Enum value 2.  |  |



[Go to Top](#structure)

### <a name='structure-class_enum' /> public structure::class_enum

class enum 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| one | Enum value 1.  |  |
| two | Enum value 2.  |  |



#### Qualifiers: 
* strong


[Go to Top](#structure)

### <a name='structure-class_enum_inheritance' /> public structure::class_enum_inheritance

class enum with inheritance 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| one | Enum value 1.  |  |
| two | Enum value 2.  |  |



#### Inheritance: 
Inherits from int.



#### Qualifiers: 
* strong


[Go to Top](#structure)

### <a name='structure-@0' /> public structure::@0

unnamed enum 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| one | Enum value 1.  |  |
| one | Enum value 1.  |  |
| one | Enum value 1.  |  |
| two | Enum value 2.  |  |
| two | Enum value 2.  |  |
| two | Enum value 2.  |  |



[Go to Top](#structure)

## Type Definitions
### <a name='structure-this_is_a_int' /> public structure::this_is_a_int 

typedefs inside a namespace 








[Go to Top](#structure)

## Variables
### <a name='structure-public_variable' /> public structure::public_variable 

public variable 








[Go to Top](#structure)

### <a name='structure-protected_variable' /> protected structure::protected_variable 

protected variable 








[Go to Top](#structure)

### <a name='structure-private_variable' /> private structure::private_variable 

private variable 








[Go to Top](#structure)

### <a name='structure-private_variable_with_value' /> private structure::private_variable_with_value  = 0

variable with default value 








[Go to Top](#structure)

## Functions
### <a name='structure-function_without_parameters' /> public void structure::function_without_parameters ()

function without anything 








[Go to Top](#structure)

### <a name='structure-function_set_to_default' /> public void structure::function_set_to_default ()=default

function with initializer 








[Go to Top](#structure)

### <a name='structure-function_with_one_argument' /> public void structure::function_with_one_argument (int a)

function with one argument 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| int | a | the one argument of this function  |












[Go to Top](#structure)

### <a name='structure-function_with_in_argument' /> public void structure::function_with_in_argument (int a)

function with one "in" argument 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int | a | the input argument of this function  |












[Go to Top](#structure)

### <a name='structure-function_with_default_value_argument' /> public void structure::function_with_default_value_argument (int a=0)

function with one "in" argument, that has a default value 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int | a | the input argument of this function, default value 0  |












[Go to Top](#structure)

### <a name='structure-function_with_out_argument' /> public void structure::function_with_out_argument (int a)

function with one "out" argument 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| out | int | a | the one output argument of this function  |












[Go to Top](#structure)

### <a name='structure-function_with_two_arguments' /> public void structure::function_with_two_arguments (int a, int b)

function with two arguments 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int | a | the input argument of this function  |
| inout | int | b | the second, inout argument of this function  |












[Go to Top](#structure)

### <a name='structure-function_with_return_value' /> public int structure::function_with_return_value (int a)

function with return value 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | int | a | the input argument of this function |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | the return value  |












[Go to Top](#structure)

### <a name='structure-const_function' /> public void structure::const_function () const

a const function - this is hopefully similar to override and such 








#### Qualifiers: 
* const


[Go to Top](#structure)

### <a name='structure-function_with_template' /> public T structure::function_with_template (T a)

function with template 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | the template parameter of this function |

#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| T | a | input parameter of template type |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | returns also a template type  |












[Go to Top](#structure)

### <a name='structure-function_with_exception' /> public void structure::function_with_exception ()

function that throws an exception 











> **exception innerException:** this is the exception thrown by this function 




[Go to Top](#structure)

### <a name='structure-function_with_retval' /> public int structure::function_with_retval ()

function that strange retval command 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | return value - yes, we can have both!  |








> **retval 0:** this is one possible return value 



> **retval 15:** this is another possible return value




[Go to Top](#structure)

