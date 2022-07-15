"""
Parsing Doxygen XML source code representation

The module consists of the following functions:
    * parseProgramListingNode 
        - main function to parse source code
    * replaceSpaceNodes 
        - replace Doxygen's <sp> nodes by spaces
"""

import sys
import xml.etree.ElementTree as ET
import logging

def replaceSpaceNodes(node):
    """Replace Doxygen's <sp> nodes by spaces

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        the <highlight> node that contains the <sp>s

    Returns
    -------
    xml.etree.ElementTree.Element :
        The given node without <sp>
    """

    nodeAsString = ET.tostring(node, encoding='unicode')

    nodeAsString = nodeAsString.replace("<sp/>", ' ')
    nodeAsString = nodeAsString.replace("<sp />", ' ')

    node = ET.fromstring(nodeAsString)
    return node


def parseProgramListingNode(node, references):
    """Parses a <programlisting> node into text

    This function takes the <programlisting> node, iterates over <codelines> and
    extracts all text contained in <highlight> nodes - therefore deleting a lot
    of information, and only concentrating on the code itself (Markdown can
    take care of the highlighting afterwards)

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        the <programlisting> node
    references : str
        current list of references to be updated in case one is found in the
        code

    Returns
    -------
    str :
        The code as a (readable) string
    """

    import parser.parse_description as parse_description
    codeAsText = ""
    for codelineNode in node.findall('codeline'):
        if codeAsText:
            codeAsText += "\n"
        for highlightnode in codelineNode.findall('highlight'):
            highlightnode = replaceSpaceNodes(highlightnode)
            if highlightnode.text:
                codeAsText += highlightnode.text

            for inner in highlightnode:
                if inner.tag == "ref":
                    codeAsText = parse_description.parseReference(inner, codeAsText, references)
                else:
                    logging.warning("parse_code: I got an element in a codeline" +\
                        " I didn't expect: Tag = %s", inner.tag)

                if inner.tail:
                    codeAsText += inner.tail

    return codeAsText
