# <a name='regression' />  regression

Tests for things that failed in the past. 




## Type Definitions
| Name | Description | 
| ---- | ---- |
| [referenced_type](#regression-referenced_type) | typedef for reference  |
| [function_using_definition_for_copydoc](#regression-function_using_definition_for_copydoc) | this is a function type with return value and an argument  |
| [function_typedef](#regression-function_typedef) | this is a function type with return value and an argument  |
| [second_referenced_type](#regression-second_referenced_type) | typedef for reference  |


## Functions
| Name | Description | 
| ---- | ---- |
| [function_with_reference_in_code](#regression-function_with_reference_in_code) | mixing code and references  |
| [function_impl_for_using_definition](#regression-function_impl_for_using_definition) | this is the actual function that belongs to the type [regression::function_using_definition_for_copydoc][regression-function_using_definition_for_copydoc]  |
| [function_with_pointers](#regression-function_with_pointers) | name with pointers  |
| [function_with_reference_in_name](#regression-function_with_reference_in_name) | references in names  |
| [function_with_spaceless_templates](#regression-function_with_spaceless_templates) | spaceless templates  |



## Type Definitions
### <a name='regression-referenced_type' /> public regression::referenced_type 

typedef for reference 








[Go to Top](#regression)

### <a name='regression-function_using_definition_for_copydoc' /> public regression::function_using_definition_for_copydoc 

this is a function type with return value and an argument 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
|  | a | input argument |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | returns an int  |












[Go to Top](#regression)

### <a name='regression-function_typedef' /> public int (*function_typedef) (double a)

this is a function type with return value and an argument 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
|  | a | input argument |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | returns an int  |












[Go to Top](#regression)

### <a name='regression-second_referenced_type' /> public regression::second_referenced_type 

typedef for reference 








[Go to Top](#regression)

## Functions
### <a name='regression-function_with_reference_in_code' /> public void regression::function_with_reference_in_code ()

mixing code and references 










In case a small code output (larger codeblocks don't work with references in Markdown anyway - different problem!) contains a reference, the code identifiers \' have to be inside the brackets [ ] for the reference. Since these are read before the reference is evaluated, they would be outside automatically:

Code: [`regression::referenced_type`][regression-referenced_type] 




[Go to Top](#regression)

### <a name='regression-function_impl_for_using_definition' /> public int regression::function_impl_for_using_definition (double a)

this is the actual function that belongs to the type [regression::function_using_definition_for_copydoc][regression-function_using_definition_for_copydoc] 





#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| double | a | implementation of an input argument |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| int | implementation of a return type  |











[Go to Top](#regression)

### <a name='regression-function_with_pointers' /> public void regression::function_with_pointers (int *a, int *b)

name with pointers 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| int * | a | first pointer argument  |
| int * | b | second pointer argument  |







If the pointer arguments in a function appear at unfavorable places, they get converted into italic text. This should not be the case!




[Go to Top](#regression)

### <a name='regression-function_with_reference_in_name' /> public [second_referenced_type][regression-second_referenced_type] regression::function_with_reference_in_name ()

references in names 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [second_referenced_type][regression-second_referenced_type] | a referenced type  |







If a reference appears in a name, this will not automatically appear as a reference in Markdown. That typedefs are not recognized as return parameters is technically another problem though




[Go to Top](#regression)

### <a name='regression-function_with_spaceless_templates' /> public void regression::function_with_spaceless_templates (std::vector< std::string > a)

spaceless templates 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| std::vector< std::string > | a | templated argument  |







If Template parameters don't have spaces between the <> and their name, they are interpreted as html nodes in names and the <> disappear




[Go to Top](#regression)

[regression-function_using_definition_for_copydoc]:#regression-function_using_definition_for_copydoc
[regression-referenced_type]:#regression-referenced_type
[regression-second_referenced_type]:#regression-second_referenced_type
