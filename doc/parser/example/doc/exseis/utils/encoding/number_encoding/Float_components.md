# <a name='exseis-utils-encoding-number_encoding-Float_components' /> public exseis::utils::encoding::number_encoding::Float_components

The [`Float_components`][exseis-utils-encoding-number_encoding-Float_components] class represents a floating point number in terms of its components: {sign, exponent, significand}. 



The number can be found natively as std::pow(-1,sign) * (significand * std::pow(2,-32))
- std::pow(2,exponent);  




## Public Attributes
| Name | Description | 
| ---- | ---- |
| [sign](#exseis-utils-encoding-number_encoding-Float_components-sign) | The sign of the number.  |
| [exponent](#exseis-utils-encoding-number_encoding-Float_components-exponent) | The power of 2 exponent of the number.  |
| [significand](#exseis-utils-encoding-number_encoding-Float_components-significand) | The left-justified significand of the number.  |



## Public Attributes
### <a name='exseis-utils-encoding-number_encoding-Float_components-sign' /> public exseis::utils::encoding::number_encoding::Float_components::sign  = 0

The sign of the number. 








[Go to Top](#exseis-utils-encoding-number_encoding-Float_components)

### <a name='exseis-utils-encoding-number_encoding-Float_components-exponent' /> public exseis::utils::encoding::number_encoding::Float_components::exponent  = 0

The power of 2 exponent of the number. 








[Go to Top](#exseis-utils-encoding-number_encoding-Float_components)

### <a name='exseis-utils-encoding-number_encoding-Float_components-significand' /> public exseis::utils::encoding::number_encoding::Float_components::significand  = 0

The left-justified significand of the number. 








[Go to Top](#exseis-utils-encoding-number_encoding-Float_components)

[exseis-utils-encoding-number_encoding-Float_components]:./Float_components.md#exseis-utils-encoding-number_encoding-Float_components
