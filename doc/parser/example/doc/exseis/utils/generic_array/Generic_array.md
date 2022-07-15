# <a name='exseis-utils-generic_array-Generic_array' /> public exseis::utils::generic_array::Generic_array

A type-erased array. 



This array provides an interface using one type, but stores the data using another.



> **see:** make_generic_array 



## Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | InterfaceType | The type used in the get/set interfaces. It should be capable of representing the values of the underlying array. |


## Classes
| Name | Description | 
| ---- | ---- |
| [Concept](./Generic_array/Concept.md) | Abstract base class to wrap the underlying array.  |
| [Model](./Generic_array/Model.md) | Concrete instance of the abstract [Concept][exseis-utils-generic_array-Generic_array-Concept] class, implementing storing the array of type StoredArray and providing an interface using the InterfaceType.  |


## Special Member Functions
| Name | Description | 
| ---- | ---- |
| [Generic_array](#exseis-utils-generic_array-Generic_array-Generic_array) | Copy constructor: value semantics.  |
| [operator=](#exseis-utils-generic_array-Generic_array-operator=) | Copy assignment: value semantics.  |
| [Generic_array](#exseis-utils-generic_array-Generic_array-Generic_array-1) | Move constructor: default.  |
| [operator=](#exseis-utils-generic_array-Generic_array-operator=-1) | Move assignment: default.  |
| [~Generic_array](#exseis-utils-generic_array-Generic_array-~Generic_array) | Default destructor.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [value_type](#exseis-utils-generic_array-Generic_array-value_type) | The value type accepted and returned by get/set interfaces.  |
| [size_type](#exseis-utils-generic_array-Generic_array-size_type) | The type used for indexing.  |


## Private Attributes
| Name | Description | 
| ---- | ---- |
| [m_concept](#exseis-utils-generic_array-Generic_array-m_concept) |  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Generic_array](#exseis-utils-generic_array-Generic_array-Generic_array-2) | Templated constructor, accepts an array of type `StoredArray` and uses it for the array storage.  |
| [set](#exseis-utils-generic_array-Generic_array-set) | Set a value in the array.  |
| [get](#exseis-utils-generic_array-Generic_array-get) | Get a value from the array.  |
| [type](#exseis-utils-generic_array-Generic_array-type) | Get the `Type` of the underlying data.  |
| [size](#exseis-utils-generic_array-Generic_array-size) | Get the number of entries in the array.  |
| [capacity](#exseis-utils-generic_array-Generic_array-capacity) | Get the current reserved size of the array.  |
| [data](#exseis-utils-generic_array-Generic_array-data) | Get a pointer to the underlying data.  |
| [data](#exseis-utils-generic_array-Generic_array-data-1) | Get a pointer to the underlying data.  |



## Special Member Functions
### <a name='exseis-utils-generic_array-Generic_array-Generic_array' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::Generic_array (const Generic_array &other)

Copy constructor: value semantics. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Generic_array][exseis-utils-generic_array-Generic_array] & | other | The [Generic_array][exseis-utils-generic_array-Generic_array] to copy.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-operator=' /> public [Generic_array][exseis-utils-generic_array-Generic_array] & exseis::utils::generic_array::Generic_array< InterfaceType >::operator= (const Generic_array &other)

Copy assignment: value semantics. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Generic_array][exseis-utils-generic_array-Generic_array] & | other | The [Generic_array][exseis-utils-generic_array-Generic_array] to copy.  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Generic_array][exseis-utils-generic_array-Generic_array] & | A reference to the current instance.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-Generic_array-1' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::Generic_array (Generic_array &&)=default

Move constructor: default. 








[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-operator=-1' /> public [Generic_array][exseis-utils-generic_array-Generic_array] & exseis::utils::generic_array::Generic_array< InterfaceType >::operator= (Generic_array &&)=default

Move assignment: default. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Generic_array][exseis-utils-generic_array-Generic_array] & | A reference to the current instance.  |












[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-~Generic_array' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::~Generic_array ()=default

Default destructor. 








[Go to Top](#exseis-utils-generic_array-Generic_array)

## Public Types
### <a name='exseis-utils-generic_array-Generic_array-value_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::value_type 

The value type accepted and returned by get/set interfaces. 








[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-size_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::size_type 

The type used for indexing. 








[Go to Top](#exseis-utils-generic_array-Generic_array)

## Private Attributes
### <a name='exseis-utils-generic_array-Generic_array-m_concept' /> private exseis::utils::generic_array::Generic_array< InterfaceType >::m_concept 










A pointer to the type-erased container. This will be an instance of Model< StoredArray >. 




[Go to Top](#exseis-utils-generic_array-Generic_array)

## Public Functions
### <a name='exseis-utils-generic_array-Generic_array-Generic_array-2' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::Generic_array (StoredArray stored_array)

Templated constructor, accepts an array of type `StoredArray` and uses it for the array storage. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | StoredArray | The array type to use for the underlying storage.  |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | StoredArray | stored_array | The array to use for the underlying storage. |







The input array is taken as a copy, so a user will likely want to pass the array as an rvalue to this constructor to avoid copies either by constructing it in-place, or using std::move. e.g. 
```c++
auto generic_array =
    Generic_array< InterfaceType >(std::vector< StorageType >(size));
```

 or 

```c++
auto my_vector = std::vector< StorageType >;
my_vector.push_back(x);
my_vector.push_back(y);
my_vector.push_back(z);

auto generic_array =
    Generic_array< InterfaceType >(std::move(my_vector));
```





> **pre:** min(StoredArray::value_type) >= min(InterfaceType) 





> **pre:** max(StoredArray::value_type) <= max(InterfaceType) 





> **post:** [size()][exseis-utils-generic_array-Generic_array-size] == stored_array.size() 





> **post:** [capacity()][exseis-utils-generic_array-Generic_array-capacity] == stored_array.capacity() 




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-set' /> public void exseis::utils::generic_array::Generic_array< InterfaceType >::set (size_type index, InterfaceType value)

Set a value in the array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [size_type][exseis-utils-generic_array-Generic_array-size_type] | index | The index to set.  |
| in | InterfaceType | value | The value to set. |









> **pre:** index < [size()][exseis-utils-generic_array-Generic_array-size] 





> **post:** get(index) == value 




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-get' /> public InterfaceType exseis::utils::generic_array::Generic_array< InterfaceType >::get (size_type index) const

Get a value from the array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [size_type][exseis-utils-generic_array-Generic_array-size_type] | index | The index of the value to get. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| InterfaceType | The value at the given index. |












> **pre:** index < [size()][exseis-utils-generic_array-Generic_array-size] 




#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-type' /> public Type exseis::utils::generic_array::Generic_array< InterfaceType >::type () const

Get the `Type` of the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| Type | The `Type` of the underlying data.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-size' /> public [size_type][exseis-utils-generic_array-Generic_array-size_type] exseis::utils::generic_array::Generic_array< InterfaceType >::size () const

Get the number of entries in the array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [size_type][exseis-utils-generic_array-Generic_array-size_type] | The number of entries in the array.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-capacity' /> public [size_type][exseis-utils-generic_array-Generic_array-size_type] exseis::utils::generic_array::Generic_array< InterfaceType >::capacity () const

Get the current reserved size of the array. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| [size_type][exseis-utils-generic_array-Generic_array-size_type] | The current reserved size of the array.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-data' /> public const unsigned char * exseis::utils::generic_array::Generic_array< InterfaceType >::data () const

Get a pointer to the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| const unsigned char * | A pointer to the underlying data.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

### <a name='exseis-utils-generic_array-Generic_array-data-1' /> public unsigned char * exseis::utils::generic_array::Generic_array< InterfaceType >::data ()

Get a pointer to the underlying data. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| unsigned char * | A pointer to the underlying data.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array)

[exseis-utils-generic_array-Generic_array]:./Generic_array.md
[exseis-utils-generic_array-Generic_array-Concept]:./Generic_array/Concept.md
[exseis-utils-generic_array-Generic_array-capacity]:./Generic_array.md#exseis-utils-generic_array-Generic_array-capacity
[exseis-utils-generic_array-Generic_array-size]:./Generic_array.md#exseis-utils-generic_array-Generic_array-size
[exseis-utils-generic_array-Generic_array-size_type]:./Generic_array.md#exseis-utils-generic_array-Generic_array-size_type
