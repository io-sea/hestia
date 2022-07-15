# <a name='exseis-utils-signal_processing' />  exseis::utils::signal_processing

Functions for processing seismic signals. 




## C API
| Name | Description | 
| ---- | ---- |
| [exseis_Gain_function)](#exseis-utils-signal_processing-exseis_Gain_function) | C API for [exseis::utils::signal_processing::Gain_function][exseis-utils-signal_processing-Gain_function].  |
| [exseis_Taper_function)](#exseis-utils-signal_processing-exseis_Taper_function) | C API for [exseis::utils::signal_processing::Taper_function][exseis-utils-signal_processing-Taper_function].  |
| [exseis_agc](#exseis-utils-signal_processing-exseis_agc) | C API interface for [agc][exseis-utils-signal_processing-agc].  |
| [exseis_rectangular_rms_gain](#exseis-utils-signal_processing-exseis_rectangular_rms_gain) | C API for [exseis::utils::signal_processing::rectangular_rms_gain][exseis-utils-signal_processing-rectangular_rms_gain].  |
| [exseis_triangular_rms_gain](#exseis-utils-signal_processing-exseis_triangular_rms_gain) | C API for [exseis::utils::signal_processing::triangular_rms_gain()][exseis-utils-signal_processing-triangular_rms_gain]  |
| [exseis_mean_abs_gain](#exseis-utils-signal_processing-exseis_mean_abs_gain) | C API for [exseis::utils::signal_processing::mean_abs_gain()][exseis-utils-signal_processing-mean_abs_gain]  |
| [exseis_median_gain](#exseis-utils-signal_processing-exseis_median_gain) | C API for [exseis::utils::signal_processing::median_gain()][exseis-utils-signal_processing-median_gain]  |
| [exseis_mute](#exseis-utils-signal_processing-exseis_mute) | C API for [exseis::utils::signal_processing::mute][exseis-utils-signal_processing-mute].  |
| [exseis_taper](#exseis-utils-signal_processing-exseis_taper) | C API for [exseis::utils::signal_processing::taper][exseis-utils-signal_processing-taper].  |
| [exseis_linear_taper](#exseis-utils-signal_processing-exseis_linear_taper) | C API for [exseis::utils::signal_processing::linear_taper][exseis-utils-signal_processing-linear_taper].  |
| [exseis_cosine_taper](#exseis-utils-signal_processing-exseis_cosine_taper) | C API for [exseis::utils::signal_processing::cosine_taper][exseis-utils-signal_processing-cosine_taper].  |
| [exseis_cosine_square_taper](#exseis-utils-signal_processing-exseis_cosine_square_taper) | The C API for [exseis::utils::signal_processing::cosine_square_taper][exseis-utils-signal_processing-cosine_square_taper].  |


## Gain Functions
| Name | Description | 
| ---- | ---- |
| [rectangular_rms_gain](#exseis-utils-signal_processing-rectangular_rms_gain) | Find the normalised root mean square (RMS) of a signal in a rectangular window.  |
| [triangular_rms_gain](#exseis-utils-signal_processing-triangular_rms_gain) | Find the normalised root mean square (RMS) of a signal in a triangular window.  |
| [mean_abs_gain](#exseis-utils-signal_processing-mean_abs_gain) | Find the normalised mean absolute value (MAV) of a signal in a rectangular window.  |
| [median_gain](#exseis-utils-signal_processing-median_gain) | Find the normalised median value inside a signal amplitude window.  |


## Taper Functions
| Name | Description | 
| ---- | ---- |
| [linear_taper](#exseis-utils-signal_processing-linear_taper) | A linear taper function.  |
| [cosine_taper](#exseis-utils-signal_processing-cosine_taper) | A cosine taper function.  |
| [cosine_square_taper](#exseis-utils-signal_processing-cosine_square_taper) | A cosine square taper function.  |


## Temporal Filter Types
| Name | Description | 
| ---- | ---- |
| [FltrType](#exseis-utils-signal_processing-FltrType) | An enum class of the different types of filters.  |
| [FltrDmn](#exseis-utils-signal_processing-FltrDmn) | An enum class of the different types of filtering domains.  |
| [PadType](#exseis-utils-signal_processing-PadType) | An enum class of the different types of trace padding functions.  |
| [FltrPad](#exseis-utils-signal_processing-FltrPad) | Function interface for filter padding functions.  |
| [Complex_trace_value](#exseis-utils-signal_processing-Complex_trace_value) | Complex type for traces.  |


## Temporal Filter Functions
| Name | Description | 
| ---- | ---- |
| [filter_order](#exseis-utils-signal_processing-filter_order) | Determines the filter order if given passband and stopband frequecnies.  |
| [expand_poly](#exseis-utils-signal_processing-expand_poly) | Expands a series of polynomials of the form (z-b0)(z-b1)...(z-bn)  |
| [lowpass](#exseis-utils-signal_processing-lowpass) | Creates a digital Butterworth lowpass filter for a given corner in zero/pole/gain form.  |
| [highpass](#exseis-utils-signal_processing-highpass) | Creates a digital Butterworth highpass filter for a given corner in zero/pole/gain form.  |
| [bandpass](#exseis-utils-signal_processing-bandpass) | Creates a digital Butterworth bandpass filter for a given corner in zero/pole/gain form.  |
| [bandstop](#exseis-utils-signal_processing-bandstop) | Creates a digital Butterworth bandstop filter for a given corner in zero/pole/gain form.  |
| [make_filter](#exseis-utils-signal_processing-make_filter) | Creates a discrete, digital Butterworth filter for a given corner in polynomial transfer function form.  |
| [get_pad](#exseis-utils-signal_processing-get_pad) | Get the pattern for padding traces for filtering.  |
| [filter_freq](#exseis-utils-signal_processing-filter_freq) | Filter trace in frequency domain.  |
| [filter_time](#exseis-utils-signal_processing-filter_time) | Filter trace in time domain.  |
| [temporal_filter](#exseis-utils-signal_processing-temporal_filter) | Temporally filter traces when given passband and stopband frequencies.  |
| [temporal_filter](#exseis-utils-signal_processing-temporal_filter-1) | Temporally filter traces when given passband frequencies and filter Order.  |


## Type Definitions
| Name | Description | 
| ---- | ---- |
| [Gain_function](#exseis-utils-signal_processing-Gain_function) | Function type for signal scaling/normalization in the agc function.  |
| [Taper_function](#exseis-utils-signal_processing-Taper_function) | A function for tapering a signal at one or both ends.  |


## Functions
| Name | Description | 
| ---- | ---- |
| [agc](#exseis-utils-signal_processing-agc) | Apply automatic gain control to a set of tapers --> used for actual operation during output.  |
| [mute](#exseis-utils-signal_processing-mute) | Apply mutes and taper to a signal.  |
| [taper](#exseis-utils-signal_processing-taper) | Apply a taper to a signal.  |



## C API
### <a name='exseis-utils-signal_processing-exseis_Gain_function' /> public exseis::utils::signal_processing::exseis_Gain_function) )(const exseis_Trace_value *signal, size_t window_size, exseis_Trace_value target_amplitude, size_t window_center)

C API for [exseis::utils::signal_processing::Gain_function][exseis-utils-signal_processing-Gain_function]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in |  | signal | An array of signal amplitudes  |
| in |  | window_size | The window size of the gain function.  |
| in |  | target_amplitude | Value to which signal is normalised.  |
| in |  | window_center | Index of the center of the window. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | The normalization factor for the signal value. |







Function type for signal scaling/normalization in the agc function. 
The value returned by this function should be a factor that will normalize a signal at `signal[window_center]` to around the value `target_amplitude`, smoothed over a window of size `window_size`.

That is, for the `j`th element of `signal`, and a `window_size` of `2*half_win+1`, the scaling will be applied equivalent to: 
```c++
signal[j] *= Gain_function(
    &signal[j - half_win], window_size,
    target_amplitude, j);
```

When the window extends outside the bounds of the `signal` array, it is truncated to the size that is within the bounds of `signal`. The `window_center` is an index into `signal` to the center of the un-truncated window.




> **remark:** C API: [exseis_Gain_function][exseis-utils-signal_processing-exseis_Gain_function] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_Taper_function' /> public exseis::utils::signal_processing::exseis_Taper_function) )(exseis_Trace_value position, exseis_Trace_value width)

C API for [exseis::utils::signal_processing::Taper_function][exseis-utils-signal_processing-Taper_function]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in |  | position | The current position of the taper (between 0 and `width`).  |
| in |  | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | The taper scaling, in range [0, 1]. |







A function for tapering a signal at one or both ends. A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_agc' /> public void exseis::utils::signal_processing::exseis_agc (size_t signal_size, exseis_Trace_value *signal, exseis_Gain_function gain_function, size_t window_size, exseis_Trace_value target_amplitude)

C API interface for [agc][exseis-utils-signal_processing-agc]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in a signal  |
| inout | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | Array of the signal data.  |
| in | [exseis_Gain_function][exseis-utils-signal_processing-exseis_Gain_function] | gain_function | Statistical function which returns a scaled value for a signal.  |
| in | size_t | window_size | Length of the agc window  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | target_amplitude | Value to which signal are normalised |







Apply automatic gain control to a set of tapers --> used for actual operation during output. 

> **remark:** C API: [exseis_agc][exseis-utils-signal_processing-exseis_agc] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_rectangular_rms_gain' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_rectangular_rms_gain (const exseis_Trace_value *signal, size_t window_size, exseis_Trace_value target_amplitude, size_t window_center)

C API for [exseis::utils::signal_processing::rectangular_rms_gain][exseis-utils-signal_processing-rectangular_rms_gain]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | An array of signal amplitudes  |
| in | size_t | window_size | The window size of the gain function.  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | target_amplitude | Value to which signal is normalised.  |
| in | size_t | window_center | Index of the center of the window. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The normalised signal value using RMS |







Find the normalised root mean square (RMS) of a signal in a rectangular window. 




> **remark:** C API: [exseis_rectangular_rms_gain()][exseis-utils-signal_processing-exseis_rectangular_rms_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_triangular_rms_gain' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_triangular_rms_gain (const exseis_Trace_value *signal, size_t window_size, exseis_Trace_value target_amplitude, size_t window_center)

C API for [exseis::utils::signal_processing::triangular_rms_gain()][exseis-utils-signal_processing-triangular_rms_gain] 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The normalised signal value using RMS with a triangular window |







Find the normalised root mean square (RMS) of a signal in a triangular window. 




> **remark:** C API: [exseis_triangular_rms_gain()][exseis-utils-signal_processing-exseis_triangular_rms_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_mean_abs_gain' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_mean_abs_gain (const exseis_Trace_value *signal, size_t window_size, exseis_Trace_value target_amplitude, size_t window_center)

C API for [exseis::utils::signal_processing::mean_abs_gain()][exseis-utils-signal_processing-mean_abs_gain] 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The normalised signal value using MAV |







Find the normalised mean absolute value (MAV) of a signal in a rectangular window. 




> **remark:** C API: [exseis_mean_abs_gain()][exseis-utils-signal_processing-exseis_mean_abs_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_median_gain' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_median_gain (const exseis_Trace_value *signal, size_t window_size, exseis_Trace_value target_amplitude, size_t window_center)

C API for [exseis::utils::signal_processing::median_gain()][exseis-utils-signal_processing-median_gain] 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The normalised median signal value. |







Find the normalised median value inside a signal amplitude window. 




> **remark:** C API: [exseis_median_gain()][exseis-utils-signal_processing-exseis_median_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_mute' /> public void exseis::utils::signal_processing::exseis_mute (size_t signal_size, exseis_Trace_value *signal, exseis_Taper_function taper_function, size_t mute_size_at_begin, size_t taper_size_at_begin, size_t taper_size_at_end, size_t mute_size_at_end)

C API for [exseis::utils::signal_processing::mute][exseis-utils-signal_processing-mute]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in the signal  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | An array of amplitudes  |
| in | [exseis_Taper_function][exseis-utils-signal_processing-exseis_Taper_function] | taper_function | Weight function for the taper ramp  |
| in | size_t | mute_size_at_begin | Length of the begin mute  |
| in | size_t | taper_size_at_begin | Length of the begin tail of the taper  |
| in | size_t | mute_size_at_end | Length of the end mute  |
| in | size_t | taper_size_at_end | Length of the end tail of the taper |







Apply mutes and taper to a signal. This mute function sets regions at the start and end of a signal to zero as specified by the user.

Following the implentation of these mute regions, a taper_function will apply a taper beginning from the first non-zero entry.



> **note:** C API: exseis_mute 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_taper' /> public void exseis::utils::signal_processing::exseis_taper (size_t signal_size, exseis_Trace_value *signal, exseis_Taper_function taper_function, size_t taper_size_at_begin, size_t taper_size_at_end)

C API for [exseis::utils::signal_processing::taper][exseis-utils-signal_processing-taper]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in the signal  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] * | signal | An array of amplitudes  |
| in | [exseis_Taper_function][exseis-utils-signal_processing-exseis_Taper_function] | taper_function | Weight function for the taper ramp  |
| in | size_t | taper_size_at_begin | Length of the begin tail of the taper  |
| in | size_t | taper_size_at_end | Length of the end tail of the taper |







Apply a taper to a signal. This taper_function will drop any initial zeros in the signal and apply the taper beginning from the first non-zero entry.



> **remark:** C API: exseis_taper 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_linear_taper' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_linear_taper (exseis_Trace_value position, exseis_Trace_value width)

C API for [exseis::utils::signal_processing::linear_taper][exseis-utils-signal_processing-linear_taper]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The taper scaling, in range [0, 1]. |







A linear taper function. A linear interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_linear_taper][exseis-utils-signal_processing-exseis_linear_taper] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_cosine_taper' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_cosine_taper (exseis_Trace_value position, exseis_Trace_value width)

C API for [exseis::utils::signal_processing::cosine_taper][exseis-utils-signal_processing-cosine_taper]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The taper scaling, in range [0, 1]. |







A cosine taper function. A cosine interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_cosine_taper][exseis-utils-signal_processing-exseis_cosine_taper] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-exseis_cosine_square_taper' /> public [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] exseis::utils::signal_processing::exseis_cosine_square_taper (exseis_Trace_value position, exseis_Trace_value width)

The C API for [exseis::utils::signal_processing::cosine_square_taper][exseis-utils-signal_processing-cosine_square_taper]. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [exseis_Trace_value][exseis-utils-types-exseis_Trace_value] | The taper scaling, in range [0, 1]. |







A cosine square taper function. A cos^2 interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_cosine_square_taper][exseis-utils-signal_processing-exseis_cosine_square_taper] 




[Go to Top](#exseis-utils-signal_processing)

## Gain Functions
### <a name='exseis-utils-signal_processing-rectangular_rms_gain' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::rectangular_rms_gain (const Trace_value *signal, size_t window_size, Trace_value target_amplitude, size_t window_center)

Find the normalised root mean square (RMS) of a signal in a rectangular window. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Trace_value][exseis-utils-types-Trace_value] * | signal | An array of signal amplitudes  |
| in | size_t | window_size | The window size of the gain function.  |
| in | [Trace_value][exseis-utils-types-Trace_value] | target_amplitude | Value to which signal is normalised.  |
| in | size_t | window_center | Index of the center of the window. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The normalised signal value using RMS |












> **remark:** C API: [exseis_rectangular_rms_gain()][exseis-utils-signal_processing-exseis_rectangular_rms_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-triangular_rms_gain' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::triangular_rms_gain (const Trace_value *signal, size_t window_size, Trace_value target_amplitude, size_t window_center)

Find the normalised root mean square (RMS) of a signal in a triangular window. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Trace_value][exseis-utils-types-Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [Trace_value][exseis-utils-types-Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The normalised signal value using RMS with a triangular window |












> **remark:** C API: [exseis_triangular_rms_gain()][exseis-utils-signal_processing-exseis_triangular_rms_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-mean_abs_gain' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::mean_abs_gain (const Trace_value *signal, size_t window_size, Trace_value target_amplitude, size_t window_center)

Find the normalised mean absolute value (MAV) of a signal in a rectangular window. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Trace_value][exseis-utils-types-Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [Trace_value][exseis-utils-types-Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The normalised signal value using MAV |












> **remark:** C API: [exseis_mean_abs_gain()][exseis-utils-signal_processing-exseis_mean_abs_gain] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-median_gain' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::median_gain (const Trace_value *signal, size_t window_size, Trace_value target_amplitude, size_t window_center)

Find the normalised median value inside a signal amplitude window. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Trace_value][exseis-utils-types-Trace_value] * | signal | Signal amplitudes  |
| in | size_t | window_size | Window length  |
| in | [Trace_value][exseis-utils-types-Trace_value] | target_amplitude | Value to which traces are normalised  |
| in | size_t | window_center | Window center iterator |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The normalised median signal value. |












> **remark:** C API: [exseis_median_gain()][exseis-utils-signal_processing-exseis_median_gain] 




[Go to Top](#exseis-utils-signal_processing)

## Taper Functions
### <a name='exseis-utils-signal_processing-linear_taper' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::linear_taper (Trace_value position, Trace_value width)

A linear taper function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_value][exseis-utils-types-Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [Trace_value][exseis-utils-types-Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The taper scaling, in range [0, 1]. |







A linear interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_linear_taper][exseis-utils-signal_processing-exseis_linear_taper] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-cosine_taper' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::cosine_taper (Trace_value position, Trace_value width)

A cosine taper function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_value][exseis-utils-types-Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [Trace_value][exseis-utils-types-Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The taper scaling, in range [0, 1]. |







A cosine interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_cosine_taper][exseis-utils-signal_processing-exseis_cosine_taper] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-cosine_square_taper' /> public [Trace_value][exseis-utils-types-Trace_value] exseis::utils::signal_processing::cosine_square_taper (Trace_value position, Trace_value width)

A cosine square taper function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [Trace_value][exseis-utils-types-Trace_value] | position | The current position of the taper (between 0 and `width`).  |
| in | [Trace_value][exseis-utils-types-Trace_value] | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [Trace_value][exseis-utils-types-Trace_value] | The taper scaling, in range [0, 1]. |







A cos^2 interpolation between 0 and 1.

A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width





> **remark:** C API: [exseis_cosine_square_taper][exseis-utils-signal_processing-exseis_cosine_square_taper] 




[Go to Top](#exseis-utils-signal_processing)

## Temporal Filter Types
### <a name='exseis-utils-signal_processing-FltrType' /> public exseis::utils::signal_processing::FltrType

An enum class of the different types of filters. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Lowpass | Create Lowpass iir Butterworth filter.  |  |
| Highpass | Create Highpass iir Butterworth filter.  |  |
| Bandpass | Create Bandpass iir Butterworth filter.  |  |
| Bandstop | Create Bandstop iir Butterworth filter.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-FltrDmn' /> public exseis::utils::signal_processing::FltrDmn

An enum class of the different types of filtering domains. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Time | Filter in time domain.  |  |
| Freq | Filter in frequency domain.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-PadType' /> public exseis::utils::signal_processing::PadType

An enum class of the different types of trace padding functions. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Zero | Pad using zeros.  |  |
| Symmetric | Pad using reflection of trace.  |  |
| Replicate | Pad using closest value in trace.  |  |
| Cyclic | Pad using values from other end of trace.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-FltrPad' /> public exseis::utils::signal_processing::FltrPad 

Function interface for filter padding functions. 








[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-Complex_trace_value' /> public exseis::utils::signal_processing::Complex_trace_value 

Complex type for traces. 








[Go to Top](#exseis-utils-signal_processing)

## Temporal Filter Functions
### <a name='exseis-utils-signal_processing-filter_order' /> public size_t exseis::utils::signal_processing::filter_order (exseis::utils::Trace_value corner_p, exseis::utils::Trace_value corner_s)

Determines the filter order if given passband and stopband frequecnies. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | exseis::utils::Trace_value | corner_p | Passband corner  |
| in | exseis::utils::Trace_value | corner_s | Stopband corner  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| size_t | Filter order  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-expand_poly' /> public void exseis::utils::signal_processing::expand_poly (const Complex_trace_value *coef, size_t nvx, exseis::utils::Trace_value *poly)

Expands a series of polynomials of the form (z-b0)(z-b1)...(z-bn) 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | const [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | coef | Vector of b coefficients  |
| in | size_t | nvx | Number of b coefficients  |
| in | exseis::utils::Trace_value * | poly | Expanded polynomial coefficients  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-lowpass' /> public exseis::utils::Trace_value exseis::utils::signal_processing::lowpass (size_t n, Complex_trace_value *z, Complex_trace_value *p, exseis::utils::Trace_value cf1)

Creates a digital Butterworth lowpass filter for a given corner in zero/pole/gain form. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | n | Filter order  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | z | Vector of filter zeros  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | p | Vector of filter poles  |
| in | exseis::utils::Trace_value | cf1 | Corner passband frequency (Hz)  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Trace_value | DOCUMENT ME  |












> **[Todo][todo]:** DOCUMENT return type 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-highpass' /> public exseis::utils::Trace_value exseis::utils::signal_processing::highpass (size_t n, Complex_trace_value *z, Complex_trace_value *p, exseis::utils::Trace_value cf1)

Creates a digital Butterworth highpass filter for a given corner in zero/pole/gain form. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | n | Filter order  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | z | Vector of filter zeros  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | p | Vector of filter poles  |
| in | exseis::utils::Trace_value | cf1 | Corner passband frequency (Hz)  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Trace_value | DOCUMENT ME  |












> **[Todo][todo]:** DOCUMENT return type 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-bandpass' /> public exseis::utils::Trace_value exseis::utils::signal_processing::bandpass (size_t n, Complex_trace_value *z, Complex_trace_value *p, exseis::utils::Trace_value cf1, exseis::utils::Trace_value cf2)

Creates a digital Butterworth bandpass filter for a given corner in zero/pole/gain form. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | n | Filter order  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | z | Vector of filter zeros  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | p | Vector of filter poles  |
| in | exseis::utils::Trace_value | cf1 | Left corner passband frequency (Hz)  |
| in | exseis::utils::Trace_value | cf2 | Right corner passband frequecy (Hz)  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Trace_value | DOCUMENT ME  |












> **[Todo][todo]:** DOCUMENT return type 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-bandstop' /> public exseis::utils::Trace_value exseis::utils::signal_processing::bandstop (size_t n, Complex_trace_value *z, Complex_trace_value *p, exseis::utils::Trace_value cf1, exseis::utils::Trace_value cf2)

Creates a digital Butterworth bandstop filter for a given corner in zero/pole/gain form. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | n | Filter order  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | z | Vector of filter zeros  |
| in | [Complex_trace_value][exseis-utils-signal_processing-Complex_trace_value] * | p | Vector of filter poles  |
| in | exseis::utils::Trace_value | cf1 | Left corner passband frequency (Hz)  |
| in | exseis::utils::Trace_value | cf2 | Right corner passband frequecy (Hz)  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| exseis::utils::Trace_value | DOCUMENT ME  |












> **[Todo][todo]:** DOCUMENT return type 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-make_filter' /> public void exseis::utils::signal_processing::make_filter (FltrType type, exseis::utils::Trace_value *numer, exseis::utils::Trace_value *denom, exseis::utils::Integer n, exseis::utils::Trace_value fs, exseis::utils::Trace_value cf1, exseis::utils::Trace_value cf2)

Creates a discrete, digital Butterworth filter for a given corner in polynomial transfer function form. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [FltrType][exseis-utils-signal_processing-FltrType] | type | Type of filter (lowpass, highpass, bandstop, bandpass)  |
| in | exseis::utils::Trace_value * | numer | Array of polynomial coefficiences in the numerator of filter transfer function  |
| in | exseis::utils::Trace_value * | denom | Array of polynomial coefficiences in the denominator of filter transfer function  |
| in | exseis::utils::Integer | n | Filter order  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | exseis::utils::Trace_value | cf1 | Left corner passband frequency (Hz)  |
| in | exseis::utils::Trace_value | cf2 | Right corner passband frequency (Hz)  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-get_pad' /> public [FltrPad][exseis-utils-signal_processing-FltrPad] exseis::utils::signal_processing::get_pad (PadType type)

Get the pattern for padding traces for filtering. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | [PadType][exseis-utils-signal_processing-PadType] | type | Type of padding  |

#### Returns: 
| Type | Description | 
| ---- | ---- |
| [FltrPad][exseis-utils-signal_processing-FltrPad] | function for padding trace  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-filter_freq' /> public void exseis::utils::signal_processing::filter_freq (size_t nss, exseis::utils::Trace_value *trc_x, exseis::utils::Trace_value fs, size_t n, exseis::utils::Trace_value *numer, exseis::utils::Trace_value *denom, FltrPad padding)

Filter trace in frequency domain. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nss | Number of Subtrace Samples  |
| in | exseis::utils::Trace_value * | trc_x | Unfiltered windowed and padded trace  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | size_t | n | Filter Order  |
| in | exseis::utils::Trace_value * | numer | Array of polynomial coefficiences in the numerator of filter transfer function  |
| in | exseis::utils::Trace_value * | denom | Array of polynomial coefficiences in the denominator of filter transfer function  |
| in | [FltrPad][exseis-utils-signal_processing-FltrPad] | padding | Funtion for padding trace  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-filter_time' /> public void exseis::utils::signal_processing::filter_time (size_t nw, exseis::utils::Trace_value *trc_orgnl, size_t n, exseis::utils::Trace_value *numer, exseis::utils::Trace_value *denom, FltrPad padding)

Filter trace in time domain. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nw | Size of filter window  |
| in | exseis::utils::Trace_value * | trc_orgnl | Unfiltered windowed and padded trace  |
| in | size_t | n | Filter Order  |
| in | exseis::utils::Trace_value * | numer | Array of polynomial coefficiences in the numerator of filter transfer function  |
| in | exseis::utils::Trace_value * | denom | Array of polynomial coefficiences in the denominator of filter transfer function  |
| in | [FltrPad][exseis-utils-signal_processing-FltrPad] | padding | Funtion for padding trace  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-temporal_filter' /> public void exseis::utils::signal_processing::temporal_filter (size_t nt, size_t ns, exseis::utils::Trace_value *trc, exseis::utils::Trace_value fs, FltrType type, FltrDmn domain, PadType pad, size_t nw, size_t win_cntr, std::vector< exseis::utils::Trace_value > corners)

Temporally filter traces when given passband and stopband frequencies. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | Number of traces  |
| in | size_t | ns | Number of samples per trace  |
| in | exseis::utils::Trace_value * | trc | Traces  |
| in | exseis::utils::Trace_value | fs | Sampling frequency  |
| in | [FltrType][exseis-utils-signal_processing-FltrType] | type | Type of filter  |
| in | [FltrDmn][exseis-utils-signal_processing-FltrDmn] | domain | Domain to filter in (frequency or time)  |
| in | [PadType][exseis-utils-signal_processing-PadType] | pad | Type of trace padding  |
| in | size_t | nw | Size of filter window  |
| in | size_t | win_cntr | Center of filter window  |
| in | std::vector< exseis::utils::Trace_value > | corners | Vector of corner frequencies (Hz)  |












[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-temporal_filter-1' /> public void exseis::utils::signal_processing::temporal_filter (size_t nt, size_t ns, exseis::utils::Trace_value *trc, exseis::utils::Trace_value fs, FltrType type, FltrDmn domain, PadType pad, size_t nw, size_t win_cntr, std::vector< exseis::utils::Trace_value > corners, size_t n)

Temporally filter traces when given passband frequencies and filter Order. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | nt | Number of traces  |
| in | size_t | ns | Number of samples per trace  |
| in | exseis::utils::Trace_value * | trc | Traces  |
| in | exseis::utils::Trace_value | fs | Sampling Frequency  |
| in | [FltrType][exseis-utils-signal_processing-FltrType] | type | Type of filter  |
| in | [FltrDmn][exseis-utils-signal_processing-FltrDmn] | domain | Domain to filter in (frequency or time)  |
| in | [PadType][exseis-utils-signal_processing-PadType] | pad | Type of trace padding  |
| in | size_t | nw | Size of filter window  |
| in | size_t | win_cntr | Center of filter window  |
| in | size_t | n | Filter order  |
| in | std::vector< exseis::utils::Trace_value > | corners | Vector of corner frequencies (Hz)  |












[Go to Top](#exseis-utils-signal_processing)

## Type Definitions
### <a name='exseis-utils-signal_processing-Gain_function' /> public exseis::utils::signal_processing::Gain_function 

Function type for signal scaling/normalization in the agc function. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in |  | signal | An array of signal amplitudes  |
| in |  | window_size | The window size of the gain function.  |
| in |  | target_amplitude | Value to which signal is normalised.  |
| in |  | window_center | Index of the center of the window. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | The normalization factor for the signal value. |








The value returned by this function should be a factor that will normalize a signal at `signal[window_center]` to around the value `target_amplitude`, smoothed over a window of size `window_size`.

That is, for the `j`th element of `signal`, and a `window_size` of `2*half_win+1`, the scaling will be applied equivalent to: 
```c++
signal[j] *= Gain_function(
    &signal[j - half_win], window_size,
    target_amplitude, j);
```

When the window extends outside the bounds of the `signal` array, it is truncated to the size that is within the bounds of `signal`. The `window_center` is an index into `signal` to the center of the un-truncated window.




> **remark:** C API: [exseis_Gain_function][exseis-utils-signal_processing-exseis_Gain_function] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-Taper_function' /> public exseis::utils::signal_processing::Taper_function 

A function for tapering a signal at one or both ends. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in |  | position | The current position of the taper (between 0 and `width`).  |
| in |  | width | The total width of the taper. |

#### Returns: 
| Type | Description | 
| ---- | ---- |
|  | The taper scaling, in range [0, 1]. |







A taper function should be 0 when `position` is 0, and 1 when `position == width`.






> **pre:** position > 0 





> **pre:** width > 0 





> **pre:** position <= width 




[Go to Top](#exseis-utils-signal_processing)

## Functions
### <a name='exseis-utils-signal_processing-agc' /> public void exseis::utils::signal_processing::agc (size_t signal_size, Trace_value *signal, Gain_function gain_function, size_t window_size, Trace_value target_amplitude)

Apply automatic gain control to a set of tapers --> used for actual operation during output. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in a signal  |
| inout | [Trace_value][exseis-utils-types-Trace_value] * | signal | Array of the signal data.  |
| in | [Gain_function][exseis-utils-signal_processing-Gain_function] | gain_function | Statistical function which returns a scaled value for a signal.  |
| in | size_t | window_size | Length of the agc window  |
| in | [Trace_value][exseis-utils-types-Trace_value] | target_amplitude | Value to which signal are normalised |









> **remark:** C API: [exseis_agc][exseis-utils-signal_processing-exseis_agc] 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-mute' /> public void exseis::utils::signal_processing::mute (size_t signal_size, Trace_value *signal, Taper_function taper_function, size_t mute_size_at_begin, size_t taper_size_at_begin, size_t mute_size_at_end, size_t taper_size_at_end)

Apply mutes and taper to a signal. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in the signal  |
| in | [Trace_value][exseis-utils-types-Trace_value] * | signal | An array of amplitudes  |
| in | [Taper_function][exseis-utils-signal_processing-Taper_function] | taper_function | Weight function for the taper ramp  |
| in | size_t | mute_size_at_begin | Length of the begin mute  |
| in | size_t | taper_size_at_begin | Length of the begin tail of the taper  |
| in | size_t | mute_size_at_end | Length of the end mute  |
| in | size_t | taper_size_at_end | Length of the end tail of the taper |







This mute function sets regions at the start and end of a signal to zero as specified by the user.

Following the implentation of these mute regions, a taper_function will apply a taper beginning from the first non-zero entry.



> **note:** C API: exseis_mute 




[Go to Top](#exseis-utils-signal_processing)

### <a name='exseis-utils-signal_processing-taper' /> public void exseis::utils::signal_processing::taper (size_t signal_size, Trace_value *signal, Taper_function taper_function, size_t taper_size_at_begin, size_t taper_size_at_end)

Apply a taper to a signal. 




#### Parameters: 
| Direction | Type | Name | Description | 
| ---- | ---- | ---- | ---- |
| in | size_t | signal_size | The number of samples in the signal  |
| in | [Trace_value][exseis-utils-types-Trace_value] * | signal | An array of amplitudes  |
| in | [Taper_function][exseis-utils-signal_processing-Taper_function] | taper_function | Weight function for the taper ramp  |
| in | size_t | taper_size_at_begin | Length of the begin tail of the taper  |
| in | size_t | taper_size_at_end | Length of the end tail of the taper |







This taper_function will drop any initial zeros in the signal and apply the taper beginning from the first non-zero entry.



> **remark:** C API: exseis_taper 




[Go to Top](#exseis-utils-signal_processing)

[exseis-utils-signal_processing-Complex_trace_value]:./index.md#exseis-utils-signal_processing-Complex_trace_value
[exseis-utils-signal_processing-FltrDmn]:./index.md#exseis-utils-signal_processing-FltrDmn
[exseis-utils-signal_processing-FltrPad]:./index.md#exseis-utils-signal_processing-FltrPad
[exseis-utils-signal_processing-FltrType]:./index.md#exseis-utils-signal_processing-FltrType
[exseis-utils-signal_processing-Gain_function]:./index.md#exseis-utils-signal_processing-Gain_function
[exseis-utils-signal_processing-PadType]:./index.md#exseis-utils-signal_processing-PadType
[exseis-utils-signal_processing-Taper_function]:./index.md#exseis-utils-signal_processing-Taper_function
[exseis-utils-signal_processing-agc]:./index.md#exseis-utils-signal_processing-agc
[exseis-utils-signal_processing-cosine_square_taper]:./index.md#exseis-utils-signal_processing-cosine_square_taper
[exseis-utils-signal_processing-cosine_taper]:./index.md#exseis-utils-signal_processing-cosine_taper
[exseis-utils-signal_processing-exseis_Gain_function]:./index.md#exseis-utils-signal_processing-exseis_Gain_function
[exseis-utils-signal_processing-exseis_Taper_function]:./index.md#exseis-utils-signal_processing-exseis_Taper_function
[exseis-utils-signal_processing-exseis_agc]:./index.md#exseis-utils-signal_processing-exseis_agc
[exseis-utils-signal_processing-exseis_cosine_square_taper]:./index.md#exseis-utils-signal_processing-exseis_cosine_square_taper
[exseis-utils-signal_processing-exseis_cosine_taper]:./index.md#exseis-utils-signal_processing-exseis_cosine_taper
[exseis-utils-signal_processing-exseis_linear_taper]:./index.md#exseis-utils-signal_processing-exseis_linear_taper
[exseis-utils-signal_processing-exseis_mean_abs_gain]:./index.md#exseis-utils-signal_processing-exseis_mean_abs_gain
[exseis-utils-signal_processing-exseis_median_gain]:./index.md#exseis-utils-signal_processing-exseis_median_gain
[exseis-utils-signal_processing-exseis_rectangular_rms_gain]:./index.md#exseis-utils-signal_processing-exseis_rectangular_rms_gain
[exseis-utils-signal_processing-exseis_triangular_rms_gain]:./index.md#exseis-utils-signal_processing-exseis_triangular_rms_gain
[exseis-utils-signal_processing-linear_taper]:./index.md#exseis-utils-signal_processing-linear_taper
[exseis-utils-signal_processing-mean_abs_gain]:./index.md#exseis-utils-signal_processing-mean_abs_gain
[exseis-utils-signal_processing-median_gain]:./index.md#exseis-utils-signal_processing-median_gain
[exseis-utils-signal_processing-mute]:./index.md#exseis-utils-signal_processing-mute
[exseis-utils-signal_processing-rectangular_rms_gain]:./index.md#exseis-utils-signal_processing-rectangular_rms_gain
[exseis-utils-signal_processing-taper]:./index.md#exseis-utils-signal_processing-taper
[exseis-utils-signal_processing-triangular_rms_gain]:./index.md#exseis-utils-signal_processing-triangular_rms_gain
[exseis-utils-types-Trace_value]:./types#exseis-utils-types-Trace_value
[exseis-utils-types-exseis_Trace_value]:./types#exseis-utils-types-exseis_Trace_value
[todo]:./../../../todo.md#todo
