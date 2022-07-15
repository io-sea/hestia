"""
Module for parsing a Doxygen XML file

Doxygen XML files have compound nodes at the highest level. These represent 
types (namespaces, classes, etc). These are created by the parse function.
Each of these compounds may have one or more sections with members. The
sections are parsed by parseSection and saved into groups, which then are 
filled with members by parseMember.

The module consists of the following functions:
    * parse
        - parse a XML file and create compounds
    * parseSection
        - parse a section in the XML file
    * parseMember 
        - parse the members in a section
"""

import sys
import xml.etree.ElementTree as ET
import logging

import containers.compounds as compounds
import helpers.settings as settings

def parseMember(member, surroundingNamespace):
    """parse a member

    This function creates a containers.compounds.Member from the node and adds
    it to the compoundDictionary of helpers.settings.

    Parameters
    ----------
    member : xml.etree.ElementTree.Element
        the <memberdef> node to parse
    surroundingNamespace : str
        the name of the namespace that belongs to the member

    Returns
    -------
    str
        The reference to the created member
    """

    currentMember = compounds.Member()

    currentMember.fill(member)
    
    # enums don't have their namespace saved in the xml
    if currentMember.surroundingNamespace == "":
        currentMember.surroundingNamespace = surroundingNamespace

    currentReference = currentMember.id
    if currentReference in settings.compoundDictionary:
        logging.warning("parse_file: This is strange... I already have" +\
            " added this member to the dictionary: %s", str(currentReference))
    settings.compoundDictionary[currentReference] = currentMember

    return currentReference


def parseSection(section, surroundingNamespace):
    """parse a section

    This function creates a containers.compounds.Group and fills it with
    Members using parseMember.

    Parameters
    ----------
    section : xml.etree.ElementTree.Element
        the <sectiondef> node to parse
    surroundingNamespace : str
        the name of the namespace that will surround all members of this section

    Returns
    -------
    containers.compounds.Group
        The created group
    """

    kind = section.get('kind')
    currentGroup = compounds.Group(settings.kindToName[kind])
    if kind == "user-defined":
        currentGroup = compounds.Group(section.findtext('header', ""))

    for memberDef in section.findall('memberdef'):
        memberReference = parseMember(memberDef, surroundingNamespace)
        currentGroup.addMember(memberReference)

    return currentGroup


def parse(filename):
    """Parse a XML file and create compounds

    This function iterates over all nodes in a file, creates 
    container.compounds.Types for all types and calls parseSection for every
    sectiondef in that compound (that then will create 
    container.compounds.Members).
    The created Types along with their Groups and Members are added to the 
    compoundDictionary of helpers.settings.

    Parameters
    ----------
    filename : str
        filename of the XML file to parse (without the path)
    """

    tree =  ET.parse(settings.pathToXML + "/" + filename)
    root = tree.getroot()

    for compoundNode in root:

        currentElement = compounds.Type()

        currentElement.fill(compoundNode)

        for section in compoundNode.findall('sectiondef'):
            newNamespace = currentElement.name
            if currentElement.surroundingNamespace != "":
                newNamespace = currentElement.surroundingNamespace + "::" + newNamespace
            newGroup = parseSection(section, newNamespace)
            currentElement.innerGroups.append(newGroup)

        currentReference = currentElement.id
        if currentReference in settings.compoundDictionary:
            logging.warning("parse_file: This is strange... I already have" +\
                " added this member to the dictionary: %s", str(currentReference))
        settings.compoundDictionary[currentReference] = currentElement
