# Doxygen XML Parser

This project can be used to convert the [Doxygen](http://www.doxygen.nl/) XML output of a C++ project into Markdown files. 
In the current state the most important things are supported and tested on one specific project.

## Project structure
The project consits of the source code in the folder `src`, some tests that do not work at all at the moment and are rather misguided in their whole idea in `test` and an example output belonging to the [ExSeisDat](https://git.ichec.ie/oilgas/ExSeisDat) project in `example`.

The source code consists of two main python executables: `xml_to_json_parser.py` and `json_to_Markdown_converter.py`:

### xml_to_json_parser
This script can be used to create a json file in the current working directory, that will contain all documented elements found in the xml folder.

Usage:
```
python3 xml_to_json_parser.py path/to/doxygen/xml output.json
```

The path must point to the folder containing the index.xml file created by Doxygen (and can be relative or absolute). The `output.json` will be the name of the JSON file created by the script.

Information on additional parameters can be accessed over `python3 xml_to_json_parser.py -h`

### json_to_Markdown_converter
After generating the json file, the data can be printed to Markdown files, following the C++ namespace logic.

Usage:
```
python3 json_to_Markdown_converter.py path/to/json/file/output.json path/to/doc/folder
```

The first argument is the output file from xml_to_json_parser, the second argument should be the folder in which the documentation will be created.

Information on additional parameters can be accessed over `python3 json_to_Markdown_converter.py -h`

## Additional Notes:

* As mentioned earlier, the tests are in a very deprecated state because I was unhappy with their whole structure. In theory they can be executed by `python3 -m unittest testtype`, where `testtype` is one of
    * structure
    * description
    * reference
    * regression
    * feature
* The scripts will create log files in the working directory with log level info. The level of verbosity and the name of the log file can be changed over additional command line arguments
* Since this is tested with only two C++ projects I expect it to break with structures, concepts and usecases I did not anticipate.
