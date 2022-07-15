# <a name='exseis-utils-encoding-number_encoding' />  exseis::utils::encoding::number_encoding

Routines for converting between different representations of number types. 



The routines in this namespace deal with conversion between endianness and conversion between number formats, e.g. IBM floating point and IEEE floating point formats. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Float_components](./Float_components.md) | The [`Float_components`][exseis-utils-encoding-number_encoding-Float_components] class represents a floating point number in terms of its components: {sign, exponent, significand}.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [from_big_endian](#exseis-utils-encoding-number_encoding-from_big_endian) | Convert an array of bytes representing a datatype in big-endian order to the equivalent datatype in host-endian order.  |
| [from_big_endian< float >](#exseis-utils-encoding-number_encoding-from_big_endian<float>) | Convert a list of bytes representing a float in big-endian order into a float in native-endianness.  |
| [from_big_endian](#exseis-utils-encoding-number_encoding-from_big_endian-1) | Convert a set of bytes representing a big-endian number into a host integer type in the host-endianness.  |
| [to_big_endian](#exseis-utils-encoding-number_encoding-to_big_endian) | Convert a host-endian integer type to an `unsigned char` array in big-endian order.  |
| [to_big_endian](#exseis-utils-encoding-number_encoding-to_big_endian-1) | Convert a host-endian integer type to an `unsigned char` array in big-endian order.  |
| [from_ibm](#exseis-utils-encoding-number_encoding-from_ibm) | Extract the sign, exponent and significand from an IBM floating point number and return it as a set of [Float_components][exseis-utils-encoding-number_encoding-Float_components].  |
| [to_float](#exseis-utils-encoding-number_encoding-to_float) | Build a native floating point representation from a set of [Float_components][exseis-utils-encoding-number_encoding-Float_components].  |
| [from_ibm_to_float](#exseis-utils-encoding-number_encoding-from_ibm_to_float) | Convert an array of bytes representing an IBM single-precision floating point number to a native single-precision float.  |



## Functions
### <a name='exseis-utils-encoding-number_encoding-from_big_endian' /> public constexpr T exseis::utils::encoding::number_encoding::from_big_endian (std::array< unsigned char, N > src)

Convert an array of bytes representing a datatype in big-endian order to the equivalent datatype in host-endian order. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::array< unsigned char, N > | src | Data in big-endian order to stuff into the datype in host-endian order. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| constexpr T | Return the requested datatype |











src[0] contains the most significant byte in big endian. src[1] contains the least significant. Shift src[0] to be in the position of the most significant byte and OR it with src[1] which is not shifted (as it is the least significant byte. 




[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-from_big_endian<float>' /> public float exseis::utils::encoding::number_encoding::from_big_endian< float > (std::array< unsigned char, 4 > src)

Convert a list of bytes representing a float in big-endian order into a float in native-endianness. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::array< unsigned char, 4 > | src | The array of bytes in big-endian order to convert. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| float | The float with native endianness.  |







This is an overloaded member function, provided for convenience. It differs from the above function only in what argument(s) it accepts.




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-from_big_endian-1' /> public T exseis::utils::encoding::number_encoding::from_big_endian (unsigned char byte, Bytes... bytes)

Convert a set of bytes representing a big-endian number into a host integer type in the host-endianness. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The host integer type to convert to.  |
| typename... | Bytes | The byte type passed in. Should be unsigned char. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | unsigned char | byte | The highest value byte to convert  |
| in | Bytes... | bytes | The rest of the bytes to convert, passed in in big-endian order. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| T | The integer type in host-endianness constructed from the big-endian ordered bytes passed in. |













This is a convenience function to avoid having to construct the `std::array`s necessary to call the other specializations.

In big-endian, higher value bytes come before lower value bytes. In the case of this function, when calling, say, `from_big_endian< int16_t >(x, y)`, The byte held by `x` will be represented in the highest 8 bits of the 16 bit integer, and `y` will be represented in the lowest 8 bits.

> **pre:** sizeof(T) == number of parameters 




[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-to_big_endian' /> public std::array< unsigned char, sizeof(T)> exseis::utils::encoding::number_encoding::to_big_endian (T src)

Convert a host-endian integer type to an `unsigned char` array in big-endian order. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The type to convert to big-endian. |

#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | T | src | The value to convert from host-endian to big-endian. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::array< unsigned char, sizeof(T)> | An array containing the bytes of `src` in big-endian order. |













For an array `dst` holding the byte-wise representation of an integer, big-endian means dst[0] holds the most significant byte and dst[3] the least. 




[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-to_big_endian-1' /> public std::array< unsigned char, sizeof(float)> exseis::utils::encoding::number_encoding::to_big_endian (float src)

Convert a host-endian integer type to an `unsigned char` array in big-endian order. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | float | src | The value to convert from host-endian to big-endian. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::array< unsigned char, sizeof(float)> | An array containing the bytes of `src` in big-endian order. |











For an array `dst` holding the byte-wise representation of an integer, big-endian means dst[0] holds the most significant byte and dst[3] the least. 




#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-from_ibm' /> public [Float_components][exseis-utils-encoding-number_encoding-Float_components] exseis::utils::encoding::number_encoding::from_ibm (std::array< unsigned char, 4 > ibm_float_bytes, bool is_big_endian)

Extract the sign, exponent and significand from an IBM floating point number and return it as a set of [Float_components][exseis-utils-encoding-number_encoding-Float_components]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::array< unsigned char, 4 > | ibm_float_bytes | The bit representation of the IBM floating point number.  |
| in | bool | is_big_endian | Whether the bit representation was read from a big endian stream. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Float_components][exseis-utils-encoding-number_encoding-Float_components] | The [Float_components][exseis-utils-encoding-number_encoding-Float_components] for the IBM number: {sign, exponent, significand}.  |












[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-to_float' /> public float exseis::utils::encoding::number_encoding::to_float (Float_components components)

Build a native floating point representation from a set of [Float_components][exseis-utils-encoding-number_encoding-Float_components]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Float_components][exseis-utils-encoding-number_encoding-Float_components] | components | The components of the floating point number: {sign, exponent, significand}. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| float | A native float equivalent to the representation in `components`. |











This function assumes the system is using IEEE 754 floating point numbers.

> **pre:** The native `float` type is IEEE 754 with round-half-to-even rounding. 




[Go to Top](#exseis-utils-encoding-number_encoding)

### <a name='exseis-utils-encoding-number_encoding-from_ibm_to_float' /> public float exseis::utils::encoding::number_encoding::from_ibm_to_float (std::array< unsigned char, 4 > ibm_float_bytes, bool is_big_endian)

Convert an array of bytes representing an IBM single-precision floating point number to a native single-precision float. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | std::array< unsigned char, 4 > | ibm_float_bytes | The byte representation of the IBM float.  |
| in | bool | is_big_endian | True if the data in `ibm_float_bytes` is in big-endian order. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| float | The corresponding 4 byte integer |











This function assumes that the system uses IEEE754. 




[Go to Top](#exseis-utils-encoding-number_encoding)

[exseis-utils-encoding-number_encoding-Float_components]:./Float_components.md#exseis-utils-encoding-number_encoding-Float_components
