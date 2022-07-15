"""
Module for parsing everything that might occur in a Doxygen description.
The description is created by parseDescription, which calls parseParaRecursive 
for every inner node and adds the descritpion it returns.
parseParaRecursive will recursively parse all inner nodes using all other 
functions of this module.

The module consists of the following functions:
    * parseDescription
        - parse a description node
    * parseParaRecursive
        - recursive function to entangle all <para> nodes
    * checkForTemplateBrackets 
        - Add spaces to < and >
    * replaceNodesWithMarkdown 
        - replace font nodes (bold, italic, etc) with Markdown 
    * fixCodeReferenceOrder
        - fix order of [] and `` to have links in code
    * parseReference
        - parse a reference node
    * addHighlightToDescription
        - add a highlighted text to the description
    * parseSimpleSect
        - parse a <simplesect> node (exceptions, conditions and return values)
    * parseProgramListing
        - call parse_code with a <programlisting> node
    * parseItemizedList
        - parse an <itemizedlist>
    * parseULink
        - parse a <ulink> node
"""

import sys
import xml.etree.ElementTree as ET
import re
import logging

import helpers.settings as settings
import parser.parse_code as parse_code
import parser.parse_todos as parse_todos

#attention: Sadly, this will destroy XML nodes
def checkForTemplateBrackets(text):
    """Add spaces to < and >

    Parameters
    ----------
    text : str
        the text in which < and > shall be spaced

    Returns
    -------
    str :
        The given text with more spaces
    """

    bracketRegex = r"((?P<bracket><)(?P<text>\w+))"
    text = re.sub(bracketRegex, "< \g<text>", text)
    bracketRegex = r"((?P<text>\w+)(?P<bracket>>))"
    text = re.sub(bracketRegex, "\g<text> >", text)
    return text

def replaceNodesWithMarkdown(node):
    """Replace font nodes with Markdown 

    Replaces <computeroutput> with `, <verbatim> with ```, <emphasis> with *,
    <bold> with ** and headings of any level with ####

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        the node in which the text shall be replaced

    Returns
    -------
    xml.etree.ElementTree.Element :
        The given node with Markdown identifiers
    """

    if(node == None):
        return node

    nodeAsString = ET.tostring(node, encoding='unicode')

    codeRegex = r"(<computeroutput>\s*)"
    nodeAsString = re.sub(codeRegex, "`", nodeAsString)
    codeRegex = r"(\s*<\/computeroutput>)"
    nodeAsString = re.sub(codeRegex, "`", nodeAsString)
    
    codeRegex = r"(<verbatim>\s*)"
    nodeAsString = re.sub(codeRegex, "\n\n```\n", nodeAsString)
    codeRegex = r"(\s*<\/verbatim>)"
    nodeAsString = re.sub(codeRegex, "\n```\n\n", nodeAsString)

    italicRegex = r"(<emphasis>\s*)"
    nodeAsString = re.sub(italicRegex, "*", nodeAsString)
    italicRegex = r"(\s*<\/emphasis>)"
    nodeAsString = re.sub(italicRegex, "*", nodeAsString)

    boldRegex = r"(<bold>\s*)"
    nodeAsString = re.sub(boldRegex, "**", nodeAsString)
    boldRegex = r"(\s*<\/bold>)"
    nodeAsString = re.sub(boldRegex, "**", nodeAsString)
    
    # Headings will be set to a fixed #### level here
    # TODO: Maybe find out, what the correct level would be?
    headingRegex = r"(<heading level=\"[0-9]\">\s*)"
    nodeAsString = re.sub(headingRegex, "#### ", nodeAsString)
    headingRegex = r"(\s*<\/heading>)"
    nodeAsString = re.sub(headingRegex, "", nodeAsString)
    
    node = ET.fromstring(nodeAsString)
    return node
    
# Markdown code icons ` and the brackets for links [] are in the wrong order
# This function searches for this occurence and changes it if neccessary
def fixCodeReferenceOrder(description):
    """Fix order of [] and `` to have links in code

    Markdown code icons ` and the brackets for links [] are in the wrong order
    This function searches for this occurence and changes it if neccessary

    Parameters
    ----------
    description : str
        the current description text

    Returns
    -------
    str :
        The updated description
    """

    problemRegex = r"(?P<text>`\[\w+((-\w+)|(::\w+))*\])(?P<link>\[\w+(-\w+)*\]`)"
    match = re.search(problemRegex, description)
    if match is not None and len(match.group("text")) > 2:
        logging.info("fixCodeReferenceOrder: Convert " + description + " ...")
        correctReference = "[`" + match.group("text")[2:-1] + "`]" + match.group("link")[:-1]
        description = description.replace(match[0], correctReference)
        logging.info("fixCodeReferenceOrder: ... to " + description)
    
    return description


def parseReference(node, description, references):
    """Parse a reference node

    Update the description with a correct Markdown reference, given the 
    information in node.

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        <ref> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated

    Returns
    -------
    str :
        The description extended by the reference link in Markdown format
    """

    refText = node.text
    if refText is None:
        refText = ""
    refText = checkForTemplateBrackets(refText)

    refid = node.get('refid')
    if refid is None:
        return description + refText
        
    if refid in settings.referenceDictionary:
        refid = settings.referenceDictionary[refid]
        references.add(refid)
        description += "[" + refText + "]" + "[" + refid + "]"
    else:
        settings.missingReferences.add(refid)
        description += refText

    return description

def addHighlightToDescription(title, description, text):
    """Add a highlighted text to the description

    Parameters
    ----------
    title : str
        The title of highlighted text
    description : str
        the current description text
    text : str
        the text to add

    Returns
    -------
    str :
        The description extended by the highlighted text
    """

    description += "> **" + title + ":** " + text + '\n'
    return description

def parseSimpleSect(node, description, references, parameterList):
    """parse a <simplesect> node (exceptions, conditions and return values)

    Parses the node and depending on the kind of section adds it as highlighted
    text or updates return parameters

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        The <simplesect> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated in case one is found
    parameterList : containers.compounds.ParameterContainer
        the parameter container of the object this description belongs to in
        case the simplesect is a return value (may be None)

    Returns
    -------
    str :
        The description (optionally extended by the highlighted text)
    """

    text = parseDescription(node, references, None)
    
    kind = node.get('kind')
    if kind == "return":
        if parameterList is not None:
            parameterList.addReturnDescription(text)
        else:
            logging.error("parse_description: Got None as parameterList when "\
                " trying to add a return description")
    else:
        description = addHighlightToDescription(kind, description, text)
        # TODO: Maybe printing the kind directly is not the most beautiful way

    return description
    

def parseProgramListing(node, description, references):
    """parse a <programlisting> node and add the code to the description

    Calls parse_code.parseProgramListingNode and adds the result as c++ 
    highlighted code to the description

    Parameters
    ----------
    node : xml.etree.ElementTree.Element
        The <programlisting> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated in case one is found

    Returns
    -------
    str :
        The description extended by the code
    """

    text = parse_code.parseProgramListingNode(node, references)
    description += "\n```c++\n" + text + "\n```\n"
    return description

def parseItemizedList(itemizedlistNode, description, references):
    """parse a <itemizedlist> node and add the list to the description

    These nodes only seem to appear in 'pages'

    Parameters
    ----------
    itemizedlistNode : xml.etree.ElementTree.Element
        The <itemizedlist> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated in case one is found

    Returns
    -------
    str :
        The description extended by the list
    """

    description += "\n"
    for listitemNode in itemizedlistNode.findall('listitem'):
        for paraNode in listitemNode.findall('para'):
            description += "- "
            description = parseParaRecursive\
                (paraNode, description, references, None)
        
        description += " \n\n"
    
    return description
    

def parseULink(ulinkNode, description):
    """parse a <ulink> node and add the link to the description

    These nodes only seem to appear in 'pages'

    Parameters
    ----------
    ulinkNode : xml.etree.ElementTree.Element
        The <ulink> node to parse
    description : str
        the current description text

    Returns
    -------
    str :
        The description extended by the link
    """

    url = ulinkNode.get('url')
    linkText = ulinkNode.text
    description += "[" + linkText + "]" + "(" + url + ")\n"
    return description
    

def parseParaRecursive(paraNode, description, references, parameterList):
    """recursive function to entangle all <para> nodes

    Every piece of description is divided up into one or more para nodes. These
    nodes are mixed XML nodes, so they can contain anything in any order. 
    This is why this is a recursive function, calling itself for every inner 
    para node and adding everything piece by piece to the description.

    This function calls all the helper functions in this module

    Parameters
    ----------
    paraNode : xml.etree.ElementTree.Element
        The current <para> node to parse
    description : str
        the current description text
    references : str
        current list of references to be updated in case one is found
    parameterList : containers.compounds.ParameterContainer
        parameter container of the calling object - may be None

    Returns
    -------
    str :
        The updated description
    """

    if(paraNode == None):
        return ""
    paraNode = replaceNodesWithMarkdown(paraNode)

    if paraNode.text:
        description += paraNode.text

    for inner in paraNode:
        if inner.tag == "ref":
            description = parseReference(inner, description, references)
        elif inner.tag == "parameterlist":
            descrString = parameterList.parseParameterList(inner, references)
            if descrString: # in case it is not a real parameter in that parameterList (exceptions, retvals)
                description += descrString + "\n"
        elif inner.tag == "simplesect":
            description = parseSimpleSect(inner, description, references, parameterList)
            description += '\n'
        elif inner.tag == "programlisting":
            description = parseProgramListing(inner, description, references)
            description += '\n'
        elif inner.tag == "xrefsect": # These are the todos
            description = parse_todos.parseTodo(inner, description, references)
            description += '\n'
        elif inner.tag == "itemizedlist":
            description = parseItemizedList(inner, description, references)
            description += '\n'
        elif inner.tag == "ulink":
            description = parseULink(inner, description)
            description += '\n'
        elif inner.tag == "variablelist":
            description = parse_todos.parseVariableList(inner, description, references)
            description += '\n'
        elif inner.tag != "anchor": # anchors are used in todo list, but not by  me
            logging.error("parse_description: Could not parse node in param: " + inner.tag)

        if inner.tail:
            description += inner.tail
            description += '\n'

    description = checkForTemplateBrackets(description)
    description = fixCodeReferenceOrder(description)
    # get rid of additional newlines at end of description
    # TODO: It is a bit uncoordinated to add and remove \n all the time, 
    # but sometimes I want them and sometimes I don't... Is there a way to clean that up?
    # also no matter which combination I choose, something always gets broken
    while len(description) > 0 and description[-1] == '\n':
        description = description[:-1]
        
    return description


def parseDescription(decriptionNode, references, parameterList):
    """Parse a description node

    This is the main function of the module. It calls parseParaRecursive for all
    <para> nodes in the description.

    Parameters
    ----------
    decriptionNode : xml.etree.ElementTree.Element
        The current description node to parse (usually <briefdescription> or 
        <detaileddescription>, but also works for other nodes containing <para>)
    references : str
        current list of references to be updated in case one is found
    parameterList : containers.compounds.ParameterContainer
        parameter container of the calling object - may be None

    Returns
    -------
    str :
        The description found in the node as readable text
    """
    description = ""
    if decriptionNode.find('para') is None:
        return description
    
    for paraNode in decriptionNode.findall('para'):
        description = parseParaRecursive\
            (paraNode, description, references, parameterList)
        description += '\n\n'

    return description
