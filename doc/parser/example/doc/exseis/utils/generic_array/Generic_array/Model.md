# <a name='exseis-utils-generic_array-Generic_array-Model' /> private exseis::utils::generic_array::Generic_array::Model

Concrete instance of the abstract [Concept][exseis-utils-generic_array-Generic_array-Concept] class, implementing storing the array of type StoredArray and providing an interface using the InterfaceType. 



Concrete instance of the abstract base class, implementing storing the array of type StoredArray and providing an interface using the InterfaceType. 




## Inheritance:
Inherits from [exseis::utils::generic_array::Generic_array< InterfaceType >::Concept][exseis-utils-generic_array-Generic_array-Concept].

## Public Types
| Name | Description | 
| ---- | ---- |
| [stored_type](#exseis-utils-generic_array-Generic_array-Model-stored_type) | The value type used by the underlying storage.  |
| [value_type](#exseis-utils-generic_array-Generic_array-Model-value_type) | The value type used by the get/set functions.  |
| [size_type](#exseis-utils-generic_array-Generic_array-Model-size_type) | The type used for indexing.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [stored_array](#exseis-utils-generic_array-Generic_array-Model-stored_array) | The underlying storage.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [Model](#exseis-utils-generic_array-Generic_array-Model-Model) | Initialize the stored_array.  |



## Public Types
### <a name='exseis-utils-generic_array-Generic_array-Model-stored_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Model< StoredArray >::stored_type 

The value type used by the underlying storage. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Model)

### <a name='exseis-utils-generic_array-Generic_array-Model-value_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Model< StoredArray >::value_type 

The value type used by the get/set functions. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Model)

### <a name='exseis-utils-generic_array-Generic_array-Model-size_type' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Model< StoredArray >::size_type 

The type used for indexing. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Model)

## Public Attributes
### <a name='exseis-utils-generic_array-Generic_array-Model-stored_array' /> public exseis::utils::generic_array::Generic_array< InterfaceType >::Model< StoredArray >::stored_array 

The underlying storage. 








[Go to Top](#exseis-utils-generic_array-Generic_array-Model)

## Public Functions
### <a name='exseis-utils-generic_array-Generic_array-Model-Model' /> public  exseis::utils::generic_array::Generic_array< InterfaceType >::Model< StoredArray >::Model (StoredArray stored_array)

Initialize the stored_array. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | StoredArray | stored_array | The array to copy/move into the underlying storage.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-generic_array-Generic_array-Model)

[exseis-utils-generic_array-Generic_array-Concept]:./Concept.md
[exseis-utils-generic_array-Generic_array-Concept-size_type]:./Concept.md#exseis-utils-generic_array-Generic_array-Concept-size_type
[exseis-utils-generic_array-Generic_array-Concept-value_type]:./Concept.md#exseis-utils-generic_array-Generic_array-Concept-value_type
