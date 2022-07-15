# <a name='exseis-utils-generic_array-Generic_array-Concept' /> public exseis::utils::generic_array::Generic_array::Concept

Abstract base class to wrap the underlying array. 




## Public Types
| Name | Description | 
| ---- | ---- |
| [value_type](#exseis-utils-generic_array-Generic_array-Concept-value_type) | The value type used in the get/set functions.  |
| [size_type](#exseis-utils-generic_array-Generic_array-Concept-size_type) | The type used for indexing.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [~Concept](#exseis-utils-generic_array-Generic_array-Concept-~Concept) | Virtual destructor.  |
| [set](#exseis-utils-generic_array-Generic_array-Concept-set) | Set a value in the array.  |
| [get](#exseis-utils-generic_array-Generic_array-Concept-get) | Get a value from the array.  |
| [type](#exseis-utils-generic_array-Generic_array-Concept-type) | Get the `Type` of the underlying data.  |
| [size](#exseis-utils-generic_array-Generic_array-Concept-size) | Get the number of entries in the array.  |
| [capacity](#exseis-utils-generic_array-Generic_array-Concept-capacity) | Get the current reserved size of the array.  |
| [data](#exseis-utils-generic_array-Generic_array-Concept-data) | Get a pointer to the underlying data.  |
| [data](#exseis-utils-generic_array-Generic_array-Concept-data-1) | Get a pointer to the underlying data.  |
| [clone](#exseis-utils-generic_array-Generic_array-Concept-clone) | Create a copy of the current instance.  |



## Public Types
### <a name='exseis-utils-generic_array-Generic_array-Concept-value_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::value_type 

The value type used in the get/set functions. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-size_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::size_type 

The type used for indexing. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

## Public Functions
### <a name='exseis-utils-generic_array-Generic_array-Concept-~Concept' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::~Concept ()=default

Virtual destructor. 








#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-set' /> public void exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::set (size_type index, value_type value)=0

Set a value in the array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] | index | The index to set.  |
| in | [value_type][exseis-utils-generic_array-Generic_array-Concept-value_type] | value | The value to set. |









> **pre:** index < [size()][exseis-utils-generic_array-Generic_array-Concept-size] 





> **post:** get(index) == value 




#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-get' /> public [value_type][exseis-utils-generic_array-Generic_array-Concept-value_type] exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::get (size_type index) const =0

Get a value from the array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] | index | The index of the value to get. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [value_type][exseis-utils-generic_array-Generic_array-Concept-value_type] | The value at the given index. |












> **pre:** index < [size()][exseis-utils-generic_array-Generic_array-Concept-size] 




#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-type' /> public Type exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::type () const =0

Get the `Type` of the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| Type | The `Type` of the underlying data.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-size' /> public [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::size () const =0

Get the number of entries in the array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] | The number of entries in the array.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-capacity' /> public [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::capacity () const =0

Get the current reserved size of the array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [size_type][exseis-utils-generic_array-Generic_array-Concept-size_type] | The current reserved size of the array.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-data' /> public const unsigned char * exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::data () const =0

Get a pointer to the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const unsigned char * | A pointer to the underlying data.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-data-1' /> public unsigned char * exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::data ()=0

Get a pointer to the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| unsigned char * | A pointer to the underlying data.  |












#### Qualifiers: 
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

### <a name='exseis-utils-generic_array-Generic_array-Concept-clone' /> public std::unique_ptr< [Concept][exseis-utils-generic_array-Generic_array-Concept] > exseis::utils::generic_array::Generic_array< InterfaceType >::Concept::clone () const =0

Create a copy of the current instance. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::unique_ptr< [Concept][exseis-utils-generic_array-Generic_array-Concept] > | A copy of the current instance.  |












#### Qualifiers: 
* const
* virtual


[Go to Top](#exseis-utils-generic_array-Generic_array-Concept)

[exseis-utils-generic_array-Generic_array]:./../Generic_array.md
[exseis-utils-generic_array-Generic_array-Concept]:./Concept.md
[exseis-utils-generic_array-Generic_array-Concept-size]:./Concept.md#exseis-utils-generic_array-Generic_array-Concept-size
[exseis-utils-generic_array-Generic_array-Concept-size_type]:./Concept.md#exseis-utils-generic_array-Generic_array-Concept-size_type
[exseis-utils-generic_array-Generic_array-Concept-value_type]:./Concept.md#exseis-utils-generic_array-Generic_array-Concept-value_type
