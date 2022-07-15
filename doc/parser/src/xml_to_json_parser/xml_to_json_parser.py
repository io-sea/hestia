"""Doxygen XML to JSON Parser

This script will parse a given XML output created by Doxygen of some C++ output
and write the contents to a JSON file.
This JSON file can then be converted to any output format.

This file can be executed as a script or loaded as a 
package exporting the functions main() and xml_to_json_parser().
"""


import sys
import xml.etree.ElementTree as ET
import json
import logging
import argparse

import containers.compounds as compounds
import helpers.settings as settings
import helpers.jsonSerialization as jS
import parser.parse_file as parse_file


def xml_to_json_parser(input_directory, output_file):
    """Parse XML files in directory and create JSON file

    Parameters
    ----------
    input_directory : str
        The path to the directory holding the index.xml file from Doxygen
    output_file : str
        The path and name of the output JSON file that should be created.
        If this file exists prior to executing this function, it will be 
        overridden.
    """

    # start setting up
    settings.init()

    # get path to index.xml
    indexFilename = "index.xml"

    if input_directory.endswith(indexFilename):
        settings.pathToXML = input_directory.replace(indexFilename, '')
        if not settings.pathToXML:
            # pick current directory if user just specified "index.xml"
            settings.pathToXML = '.'
    else:
        settings.pathToXML = input_directory


    # parse index.xml and ...
    tree = ET.parse(settings.pathToXML + "/index.xml")
    indexRoot = tree.getroot()
    # ... save all compound-references (with old references as keys and new ones as values)
    settings.setupReferences(indexRoot)

    # loop over all types referenced in index and parse them, creating objects
    # for every compound in them
    for compound in indexRoot:

        compoundKind = compound.get('kind')
        if(compoundKind != 'file' and \
            compoundKind != 'dir'):

            compoundpath = compound.get('refid') + ".xml"
            parse_file.parse(compoundpath)


    # save compound objects as json file
    file_object = open(output_file, 'w')
    json.dump(settings.compoundDictionary, file_object, cls=jS.MyEncoder, sort_keys=True, indent=4)

    for r, e in settings.referenceDictionary.items():
        logging.debug("References: r " + r + " -> " + e)

    logging.info("Missing references: %s", settings.missingReferences)


def main():
    # arguments
    parser = argparse.ArgumentParser(description='Parse the Doxygen XML ' + \
        'output, create a dictionary with all members and save it to a JSON file.')
    
    parser.add_argument('input_directory', \
        help='The directory of the Doxygen XML output. Must contain index.xml', \
        default="./xml/")
    parser.add_argument('output_file', default="output.json", \
        help='The name of the JSON file that should be created')
    parser.add_argument('-l', dest='logfile', default="xml_to_json_parser.log", \
        help='The filename for the created logfile')
    parser.add_argument('--loglevel', dest='loglevel', default='info', \
        help='The level of logging (debug, info, warning, error or critical)' + \
        ' Default is info')
    
    args = parser.parse_args()
        
    # logging
    numericLogLevel = getattr(logging, args.loglevel.upper(), None)
    if not isinstance(numericLogLevel, int):
        raise ValueError('Invalid log level: %s' % args.loglevel)
    logging.basicConfig(filename=args.logfile, filemode='w', level=numericLogLevel)
    
    logging.info("arguments: " + str(args))


    xml_to_json_parser(args.input_directory, args.output_file)


if __name__ == '__main__':
    main()
