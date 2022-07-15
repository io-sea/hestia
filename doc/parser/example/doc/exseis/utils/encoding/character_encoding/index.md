# <a name='exseis-utils-encoding-character_encoding' />  exseis::utils::encoding::character_encoding

Functions for managing and converting between character encodings. 




## Functions
| Name | Description | 
| ---- | ---- |
| [to_ascii_from_ebcdic](#exseis-utils-encoding-character_encoding-to_ascii_from_ebcdic) | Convert an EBCDIC encoded character to an ASCII encoded character.  |
| [to_ebcdic_from_ascii](#exseis-utils-encoding-character_encoding-to_ebcdic_from_ascii) | Convert an ASCII encoded character to an EBCDIC encoded character.  |
| [is_printable_ascii](#exseis-utils-encoding-character_encoding-is_printable_ascii) | Determine whether an input character is a printable ASCII character.  |
| [is_printable_ebcdic](#exseis-utils-encoding-character_encoding-is_printable_ebcdic) | Determine whether an input character is a printable EBCDIC character.  |



## Functions
### <a name='exseis-utils-encoding-character_encoding-to_ascii_from_ebcdic' /> public unsigned char exseis::utils::encoding::character_encoding::to_ascii_from_ebcdic (unsigned char ebcdic_char)

Convert an EBCDIC encoded character to an ASCII encoded character. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | unsigned char | ebcdic_char | An EBCDIC encoded character. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| unsigned char | An ASCII encoded character.  |







Returns the ASCII SUB character if there is no ASCII equivalent.




[Go to Top](#exseis-utils-encoding-character_encoding)

### <a name='exseis-utils-encoding-character_encoding-to_ebcdic_from_ascii' /> public unsigned char exseis::utils::encoding::character_encoding::to_ebcdic_from_ascii (unsigned char ascii_char)

Convert an ASCII encoded character to an EBCDIC encoded character. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | unsigned char | ascii_char | An ASCII encoded character. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| unsigned char | An EBCDIC encoded character.  |







Returns the EBCDIC SUB character if there is no EBCDIC equivalent.




[Go to Top](#exseis-utils-encoding-character_encoding)

### <a name='exseis-utils-encoding-character_encoding-is_printable_ascii' /> public bool exseis::utils::encoding::character_encoding::is_printable_ascii (unsigned char ascii_char)

Determine whether an input character is a printable ASCII character. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | unsigned char | ascii_char | The character to test. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Whether `ascii_char` represents a printable ASCII character.  |












[Go to Top](#exseis-utils-encoding-character_encoding)

### <a name='exseis-utils-encoding-character_encoding-is_printable_ebcdic' /> public bool exseis::utils::encoding::character_encoding::is_printable_ebcdic (unsigned char ebcdic_char)

Determine whether an input character is a printable EBCDIC character. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | unsigned char | ebcdic_char | The character to test. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| bool | Whether `ebcdic_char` represents a printable EBCDIC character.  |












[Go to Top](#exseis-utils-encoding-character_encoding)

