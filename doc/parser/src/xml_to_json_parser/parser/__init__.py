"""
Package for XML parse functionalities

The package is divided up into four modules:
    * parse_file 
        - main module for parsing one XML file -> calls the other modules
    * parse_description
        - parsing all elements appearing in a description node
    * parse_code
        - parsing Doxygens <programlisting> nodes, used for source code 
            representation
    * parse_todos
        - parsing Doxygens todos
"""