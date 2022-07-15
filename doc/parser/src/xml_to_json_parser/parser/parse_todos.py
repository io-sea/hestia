"""
Module for parsing Doxygen Todos

Doxygen Todos have a very unique form with a special kind of link, so the 
parsing functions can't be used for anything else and are contained in this 
module.

The module consists of the following functions:
    * parseTodo
        - parse a todo node
    * parseVariableList
        - parse the list of todos
"""

import logging


def parseTodo(node, description, references):
    """Parse a todo node

    This function gets an <xrefsect> node, which hopefully is always a todo, 
    parses the description and adds a link to the main Todo page.

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        the <xrefsect> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated

    Returns
    -------
    str
        The updated description
    """

    import parser.parse_description as parse_description
    
    if node.findtext("xreftitle") != "Todo":
        logging.error("parse_todo: Found xrefsect without Todo title!")
        return description
    innerDescriptionNode = node.find("xrefdescription")
    text = parse_description.parseDescription\
        (innerDescriptionNode, references, None)
    description = parse_description.addHighlightToDescription\
        ("[Todo][todo]", description, text)
    
    references.add("todo")

    return description

def parseVariableList(variablelistNode, description, references):
    """Parse the list of todos

    This function gets an <variablelist> node, which was as far as I could see 
    always used for the todo lists (a page with links to all todos), parses
    all inner elements, and adds their content to the description

    Parameters
    ----------
    variablelistNode : xml.etree.ElementTree.Element
        the <variablelist> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated

    Returns
    -------
    str
        The updated description
    """

    nodes = list(variablelistNode)
    import parser.parse_description as parse_description
    
    for entry in nodes:
        if entry.tag == "varlistentry":
            termNode = entry.find("term")
            if termNode:
                text = parse_description.parseParaRecursive\
                    (termNode, "", references, None)
                description += "* " + text + '\n'
        elif entry.tag == "listitem":
            for p in entry.findall("para"):
                text = parse_description.parseParaRecursive\
                    (p, "", references, None)
                description += "    * " + text + '\n'
    
    return description
    