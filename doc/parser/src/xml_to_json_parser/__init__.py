"""Doxygen XML to JSON Parser

This package provides functionality to parse Doxygen XML output of a C++ project
to a JSON file.

Given a index.xml the main function will create objects for all members, 
hierarchically ordered by the logical layout of the C++ types (basically 
namespaces-classes).
These objects will be filled with the documentation provided for the underlying 
types and then serialized into a JSON file.
Not all Doxygen functionality is supported, but the most common and useful 
possibilities are provided.

This package exports the following module:
    * xml_to_json_parser - main script and function to run the whole chain

which consist of the following subpackages:
    * containers: Container classes for the documented c++ types/functions/etc
    * parser: XML parse functionalities
    * helpers: various helper functions
"""