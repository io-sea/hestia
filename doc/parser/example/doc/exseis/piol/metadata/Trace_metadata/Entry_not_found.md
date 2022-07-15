# <a name='exseis-piol-metadata-Trace_metadata-Entry_not_found' /> public exseis::piol::metadata::Trace_metadata::Entry_not_found

An exception class thrown when a metadata entry is queried which hasn't been added to this instance. 



A common cause for this being thrown is where the key exists in the metadata structure, but the wrong type has been queried. e.g. A floating point entry for that key exists, but an integer entry for the key is requested. 




## Inheritance:
Inherits from out_of_range.


