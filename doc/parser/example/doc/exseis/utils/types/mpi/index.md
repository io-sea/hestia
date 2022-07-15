# <a name='exseis-utils-types-mpi' />  exseis::utils::types::mpi

Aliases for MPI datatypes. 




## Functions
| Name | Description | 
| ---- | ---- |
| [mpi_type](#exseis-utils-types-mpi-mpi_type) | Return the fundamental MPI datatype associated with a fundamental datatype.  |
| [mpi_type< double >](#exseis-utils-types-mpi-mpi_type<double>) | Return the MPI datatype for a `double`.  |
| [mpi_type< long double >](#exseis-utils-types-mpi-mpi_type<longdouble>) | Return the MPI datatype for a `long` double.  |
| [mpi_type< char >](#exseis-utils-types-mpi-mpi_type<char>) | Return the MPI datatype for a `long` double.  |
| [mpi_type< signed char >](#exseis-utils-types-mpi-mpi_type<signedchar>) | Return the MPI datatype for a `signed` `char`.  |
| [mpi_type< unsigned char >](#exseis-utils-types-mpi-mpi_type<unsignedchar>) | Return the MPI datatype for an `unsigned` `char`.  |
| [mpi_type< int >](#exseis-utils-types-mpi-mpi_type<int>) | Return the MPI datatype for an `int`.  |
| [mpi_type< long int >](#exseis-utils-types-mpi-mpi_type<longint>) | Return the MPI datatype for a `long` `int`.  |
| [mpi_type< long long int >](#exseis-utils-types-mpi-mpi_type<longlongint>) | Return the MPI datatype for a `long` `long` `int`.  |
| [mpi_type< unsigned int >](#exseis-utils-types-mpi-mpi_type<unsignedint>) | Return the MPI datatype for an `unsigned` `int`.  |
| [mpi_type< long unsigned int >](#exseis-utils-types-mpi-mpi_type<longunsignedint>) | Return the MPI datatype for a `long` `unsigned` `int`.  |
| [mpi_type< long long unsigned int >](#exseis-utils-types-mpi-mpi_type<longlongunsignedint>) | Return the MPI datatype for a `long` `long` `unsigned` `int`.  |
| [mpi_type< float >](#exseis-utils-types-mpi-mpi_type<float>) | Return the MPI datatype for a `float`.  |
| [mpi_type< signed short >](#exseis-utils-types-mpi-mpi_type<signedshort>) | Return the MPI datatype for a `signed` `short`.  |
| [mpi_type< unsigned short >](#exseis-utils-types-mpi-mpi_type<unsignedshort>) | Return the MPI datatype for an `unsigned` `short`.  |



## Functions
### <a name='exseis-utils-types-mpi-mpi_type' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type ()

Return the fundamental MPI datatype associated with a fundamental datatype. 




#### Template Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| typename | T | The C++ datatype  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The datatype.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<double>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< double > ()

Return the MPI datatype for a `double`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for `double`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<longdouble>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< long double > ()

Return the MPI datatype for a `long` double. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for `long` `double`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<char>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< char > ()

Return the MPI datatype for a `long` double. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `long` double.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<signedchar>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< signed char > ()

Return the MPI datatype for a `signed` `char`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `signed` `char`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<unsignedchar>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< unsigned char > ()

Return the MPI datatype for an `unsigned` `char`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `unsigned` `char`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<int>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< int > ()

Return the MPI datatype for an `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<longint>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< long int > ()

Return the MPI datatype for a `long` `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `long` `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<longlongint>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< long long int > ()

Return the MPI datatype for a `long` `long` `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `long` `long` `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<unsignedint>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< unsigned int > ()

Return the MPI datatype for an `unsigned` `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `unsigned` `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<longunsignedint>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< long unsigned int > ()

Return the MPI datatype for a `long` `unsigned` `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `long` `unsigned` `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<longlongunsignedint>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< long long unsigned int > ()

Return the MPI datatype for a `long` `long` `unsigned` `int`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `long` `long` `unsigned` `int`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<float>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< float > ()

Return the MPI datatype for a `float`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `float`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<signedshort>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< signed short > ()

Return the MPI datatype for a `signed` `short`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for a `signed` `short`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

### <a name='exseis-utils-types-mpi-mpi_type<unsignedshort>' /> public MPI_Datatype exseis::utils::types::mpi::mpi_type< unsigned short > ()

Return the MPI datatype for an `unsigned` `short`. 




#### Returns: 
| Type | Description | 
| ---- | ---- |
| MPI_Datatype | The MPI datatype for an `unsigned` `short`.  |












#### Qualifiers: 
* inline


[Go to Top](#exseis-utils-types-mpi)

