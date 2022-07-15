# <a name='exseis-utils-Enum_hash' /> public exseis::utils::Enum_hash

A hashing function for enums and enum classes. 



Older compilers don't properly specialize std::hash for enums. This class provides hashing for enums by casting them to their underlying type and calling std::hash on that.



## Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | Enum | The enum type to hash.  |


## Public Types
| Name | Description | 
| ---- | ---- |
| [Key_type](#exseis-utils-Enum_hash-Key_type) | The key type for the hash.  |
| [Underlying_type](#exseis-utils-Enum_hash-Underlying_type) | The underlying type of the enum, used for the actual hashing.  |
| [Underlying_hash](#exseis-utils-Enum_hash-Underlying_hash) | The hasher type for the underlying type.  |
| [Result_type](#exseis-utils-Enum_hash-Result_type) | The result of the underlying hasher, also the result of this hasher.  |


## Public Attributes
| Name | Description | 
| ---- | ---- |
| [underlying_hash](#exseis-utils-Enum_hash-underlying_hash) | An instance of the underlying hasher, used as the hasher for the enum values.  |


## Public Functions
| Name | Description | 
| ---- | ---- |
| [operator()](#exseis-utils-Enum_hash-operator()) | Calculate the hash of the argument.  |



## Public Types
### <a name='exseis-utils-Enum_hash-Key_type' /> public exseis::utils::Enum_hash< Enum >::Key_type 

The key type for the hash. 








[Go to Top](#exseis-utils-Enum_hash)

### <a name='exseis-utils-Enum_hash-Underlying_type' /> public exseis::utils::Enum_hash< Enum >::Underlying_type 

The underlying type of the enum, used for the actual hashing. 








[Go to Top](#exseis-utils-Enum_hash)

### <a name='exseis-utils-Enum_hash-Underlying_hash' /> public exseis::utils::Enum_hash< Enum >::Underlying_hash 

The hasher type for the underlying type. 








[Go to Top](#exseis-utils-Enum_hash)

### <a name='exseis-utils-Enum_hash-Result_type' /> public exseis::utils::Enum_hash< Enum >::Result_type 

The result of the underlying hasher, also the result of this hasher. 








[Go to Top](#exseis-utils-Enum_hash)

## Public Attributes
### <a name='exseis-utils-Enum_hash-underlying_hash' /> public exseis::utils::Enum_hash< Enum >::underlying_hash 

An instance of the underlying hasher, used as the hasher for the enum values. 








[Go to Top](#exseis-utils-Enum_hash)

## Public Functions
### <a name='exseis-utils-Enum_hash-operator()' /> public [Result_type][exseis-utils-Enum_hash-Result_type] exseis::utils::Enum_hash< Enum >::operator() (Key_type key) const

Calculate the hash of the argument. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Key_type][exseis-utils-Enum_hash-Key_type] | key | The enum value. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Result_type][exseis-utils-Enum_hash-Result_type] | Return a hash of the enum.  |












#### Qualifiers: 
* const
* inline


[Go to Top](#exseis-utils-Enum_hash)

[exseis-utils-Enum_hash-Key_type]:./Enum_hash.md#exseis-utils-Enum_hash-Key_type
[exseis-utils-Enum_hash-Result_type]:./Enum_hash.md#exseis-utils-Enum_hash-Result_type
[exseis-utils-Enum_hash-Underlying_hash]:./Enum_hash.md#exseis-utils-Enum_hash-Underlying_hash
[exseis-utils-Enum_hash-Underlying_type]:./Enum_hash.md#exseis-utils-Enum_hash-Underlying_type
