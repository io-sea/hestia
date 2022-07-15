# <a name='exseis-piol-metadata' />  exseis::piol::metadata

Seismic metadata utilities. 




## Classes
| Name | Description | 
| ---- | ---- |
| [Trace_metadata](./Trace_metadata.md) | Class for initialising the trace parameter structure and storing a structure with the necessary rules.  |


## Namespaces
| Name | Description | 
| ---- | ---- |
| [rules](./rules/index.md) | Rules associated with trace parameters.  |


## C API
| Name | Description | 
| ---- | ---- |
| [@0](#exseis-piol-metadata-@0) | C API or [exseis::piol::metadata::Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key].  |
| [exseis_Trace_metadata_key](#exseis-piol-metadata-exseis_Trace_metadata_key) | C API for [exseis::piol::metadata::Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key].  |


## Enumerations
| Name | Description | 
| ---- | ---- |
| [Trace_metadata_key](#exseis-piol-metadata-Trace_metadata_key) | The available trace parameters.  |



## C API
### <a name='exseis-piol-metadata-@0' /> public exseis::piol::metadata::@0

C API or [exseis::piol::metadata::Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key]. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| exseis_meta_Copy | Copy all headers. Don't interpret them.  |  |
| exseis_meta_ltn | Local trace number.  |  |
| exseis_meta_gtn | Global trace number.  |  |
| exseis_meta_tnl | The trace number (line)  |  |
| exseis_meta_tnr | The Trace number (record)  |  |
| exseis_meta_tn | The trace number (file)  |  |
| exseis_meta_tne | The trace number (ensemble)  |  |
| exseis_meta_ns | Number of samples in this trace.  |  |
| exseis_meta_sample_interval | The increment of this trace.  |  |
| exseis_meta_Tic | Trace identification code.  |  |
| exseis_meta_SrcNum | Source Number.  |  |
| exseis_meta_ShotNum | Shot number.  |  |
| exseis_meta_VStack | Number of traces stacked for this trace (vertical)  |  |
| exseis_meta_HStack | Number of traces stacked for this trace (horizontal)  |  |
| exseis_meta_Offset | Distance from source to receiver.  |  |
| exseis_meta_RGElev | Receiver group elevation.  |  |
| exseis_meta_SSElev | Source surface elevation.  |  |
| exseis_meta_SDElev | Source depth.  |  |
| exseis_meta_WtrDepSrc | The Water depth at source.  |  |
| exseis_meta_WtrDepRcv | The Water depth at receiver.  |  |
| exseis_meta_x_src | The source x coordinate.  |  |
| exseis_meta_y_src | The source y coordinate.  |  |
| exseis_meta_x_rcv | The receiver x coordinate.  |  |
| exseis_meta_y_rcv | The receiver y coordinate.  |  |
| exseis_meta_xCmp | The CMP x coordinate.  |  |
| exseis_meta_yCmp | The CMP y coordinate.  |  |
| exseis_meta_coordinate_scalar | The coordinate scalar value in a SEGY file.  |  |
| exseis_meta_il | The inline number.  |  |
| exseis_meta_xl | The crossline number.  |  |
| exseis_meta_TransUnit | Unit system for transduction constant.  |  |
| exseis_meta_TraceUnit | Unit system for traces.  |  |
| exseis_meta_dsdr | The sum of the differences between sources and receivers of this trace and another  |  |
| exseis_meta_Misc1 | Miscellaneous.  |  |
| exseis_meta_Misc2 | Miscellaneous.  |  |
| exseis_meta_Misc3 | Miscellaneous.  |  |
| exseis_meta_Misc4 | Miscellaneous.  |  |





The available trace parameters.




[Go to Top](#exseis-piol-metadata)

### <a name='exseis-piol-metadata-exseis_Trace_metadata_key' /> public exseis::piol::metadata::exseis_Trace_metadata_key 

C API for [exseis::piol::metadata::Trace_metadata_key][exseis-piol-metadata-Trace_metadata_key]. 










The available trace parameters.




[Go to Top](#exseis-piol-metadata)

## Enumerations
### <a name='exseis-piol-metadata-Trace_metadata_key' /> public exseis::piol::metadata::Trace_metadata_key

The available trace parameters. 






#### Enum Values: 
| Name | Description | Value | 
| ---- | ---- | ---- |
| Copy | Copy all headers. Don't interpret them.  |  |
| ltn | Local trace number.  |  |
| gtn | Global trace number.  |  |
| tnl | The trace number (line)  |  |
| tnr | The Trace number (record)  |  |
| tn | The trace number (file)  |  |
| tne | The trace number (ensemble)  |  |
| ns | Number of samples in this trace.  |  |
| sample_interval | The increment of this trace.  |  |
| Tic | Trace identification code.  |  |
| SrcNum | Source Number.  |  |
| ShotNum | Shot number.  |  |
| VStack | Number of traces stacked for this trace (vertical)  |  |
| HStack | Number of traces stacked for this trace (horizontal)  |  |
| Offset | Distance from source to receiver.  |  |
| RGElev | Receiver group elevation.  |  |
| SSElev | Source surface elevation.  |  |
| SDElev | Source depth.  |  |
| WtrDepSrc | The Water depth at source.  |  |
| WtrDepRcv | The Water depth at receiver.  |  |
| x_src | The source x coordinate.  |  |
| y_src | The source y coordinate.  |  |
| x_rcv | The receiver x coordinate.  |  |
| y_rcv | The receiver y coordinate.  |  |
| xCmp | The CMP x coordinate.  |  |
| yCmp | The CMP y coordinate.  |  |
| coordinate_scalar | The coordinate scalar in a SEGY file.  |  |
| il | The inline number.  |  |
| xl | The crossline number.  |  |
| TransUnit | Unit system for transduction constant.  |  |
| TraceUnit | Unit system for traces.  |  |
| dsdr | The sum of the differences between sources and receivers of this trace and another  |  |
| Misc1 | Miscellaneous.  |  |
| Misc2 | Miscellaneous.  |  |
| Misc3 | Miscellaneous.  |  |
| Misc4 | Miscellaneous.  |  |



#### Qualifiers: 
* strong


[Go to Top](#exseis-piol-metadata)

[exseis-piol-metadata-Trace_metadata_key]:./index.md#exseis-piol-metadata-Trace_metadata_key
