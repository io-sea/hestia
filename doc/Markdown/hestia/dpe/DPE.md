# <a name='hestia-dpe-DPE' /> public hestia::dpe::DPE

virtual key value store class to be implemented by key value store solutions 




## Public Functions
| Name | Description | 
| ---- | ---- |
| [DPE](#hestia-dpe-DPE-DPE) | default constructor  |
| [~DPE](#hestia-dpe-DPE-~DPE) | default destructor  |
| [choose_tier](#hestia-dpe-DPE-choose_tier) | Choose tier.  |
| [reorganise](#hestia-dpe-DPE-reorganise) | Move objects between tiers.  |



## Public Functions
### <a name='hestia-dpe-DPE-DPE' /> public  hestia::dpe::DPE::DPE (const std::list< struct hsm_tier > &tiers)

default constructor 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const std::list< struct [hsm_tier][hestia-hsm_tier] > & | tiers | Set of the available tiers  |












#### Qualifiers: 
* inline


[Go to Top](#hestia-dpe-DPE)

### <a name='hestia-dpe-DPE-~DPE' /> public  hestia::dpe::DPE::~DPE ()

default destructor 








#### Qualifiers: 
* inline
* virtual


[Go to Top](#hestia-dpe-DPE)

### <a name='hestia-dpe-DPE-choose_tier' /> public std::uint8_t hestia::dpe::DPE::choose_tier (const std::size_t length, const std::uint8_t hint=0)=0

Choose tier. 




#### Parameters: 
| Type | Name | Description | 
| ---- | ---- | ---- |
| const std::size_t | length | Size of the object to place  |
| const std::uint8_t | hint | Hint at the tier to place the object in |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| std::uint8_t | ID of the tier to put an object to  |







Choose one of the available tiers as the destination for an object




#### Qualifiers: 
* virtual


[Go to Top](#hestia-dpe-DPE)

### <a name='hestia-dpe-DPE-reorganise' /> public void hestia::dpe::DPE::reorganise () const =0

Move objects between tiers. 








#### Qualifiers: 
* const
* virtual


[Go to Top](#hestia-dpe-DPE)

[hestia-hsm_tier]:./../hsm_tier.md#hestia-hsm_tier
