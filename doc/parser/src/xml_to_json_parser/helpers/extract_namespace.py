"""
Extract namespaces from names

Given a complete name of an element (which might include templates, return 
values, parameters, initializers, qualifiers, etc) this module tries to extract 
the namespaces and the name of the element.

Example
-------
input:
using exseis::utils::signal_processing::Complex_trace_value = typedef std::complex<exseis::utils::Trace_value>

output:
exseis::utils::signal_processing and Complex_trace_value


The module consists of the following functions:
    * splitNamespacesAndNames - main function for splitting
    * removeTemplateBracketSpaces - Remove spaces around < and >
    * removeTemplateInnerSpaces - Replace inner spaces inside <> with -------
    * restoreTemplateSpaces - Restore spaces that have been replaced by -------
    * addPointerSpaces - Add spaces around *
"""

import re
import logging

def removeTemplateBracketSpaces(compoundName):
    """Remove spaces around < and >

    This function removes spaces like this: "< double >" -> "<double>" in case
    that existed.

    Parameters
    ----------
    compoundName : str
        The element name to change

    Returns
    -------
    str
        the (possibly updated) name
    """

    templateBracketRegex = r"(<\s*)"
    compoundName = re.sub(templateBracketRegex, "<", compoundName)
    templateBracketRegex = r"(\s*>)"
    compoundName = re.sub(templateBracketRegex, ">", compoundName)
    
    return compoundName

def removeTemplateInnerSpaces(compoundName):
    """Replace inner spaces inside <> with -------

    This function replaces spaces like this: "<long int>" -> "<long-------int>" 
    in case that existed.

    Parameters
    ----------
    compoundName : str
        The element name to change

    Returns
    -------
    str
        the (possibly updated) name
    """
    
    templateSpaceRegex = r"<(\w+\s\w+)+>"
    match = re.search(templateSpaceRegex, compoundName)
    if not match:
        return compoundName
    
    innerTemplateString = match.string[match.start():match.end()]
    innerTemplateString = innerTemplateString.replace(' ', '-------')
    compoundName = compoundName[:match.start()] + innerTemplateString \
        + compoundName[(match.end()):]
        
    return compoundName
    
def restoreTemplateSpaces(compoundName):
    """Restores inner spaces that have been replaced by -------

    This function restores spaces like this: "<long-------int>" -> "<long int>" 
    in case removeTemplateInnerSpaces replaced them before

    Parameters
    ----------
    compoundName : str
        The element name to change

    Returns
    -------
    str
        the (possibly updated) name
    """

    templateBracketRegex = r"<"
    compoundName = re.sub(templateBracketRegex, "< ", compoundName)
    templateBracketRegex = r">"
    compoundName = re.sub(templateBracketRegex, " >", compoundName)
    
    compoundName = compoundName.replace('-------', ' ')
        
    return compoundName

def addPointerSpaces(compoundName):
    """Add spaces around *

    Parameters
    ----------
    compoundName : str
        The element name to change

    Returns
    -------
    str
        the (possibly updated) name
    """

    pointerRegex = r"(?P<ptr>\*)(?P<text>\w+)"
    compoundName = re.sub(pointerRegex, "* \g<text>", compoundName)
    pointerRegex = r"(?P<text>\w+)(?P<ptr>\*)"
    compoundName = re.sub(pointerRegex, "\g<text> *", compoundName)
    
    return compoundName


def splitNamespacesAndNames(compoundName, temporaryName):
    """Splits a name containing all sorts of identifiers into namespace and name

    Given a complete name of an element (which might include templates, return 
    values, parameters, initializers, qualifiers, etc) this module tries to 
    extract the namespaces and the name of the element.

    Example
    -------
    input:
    'using exseis::utils::signal_processing::Complex_trace_value = typedef std::complex<exseis::utils::Trace_value>'

    output:
    'exseis::utils::signal_processing' and 'Complex_trace_value'

    Parameters
    ----------
    compoundName : str
        The element name to change
    temporaryName : str
        sometimes Doxygen adds a <name> node, which is a very useful hint to 
        find the correct name and namespaces. This can be given in this argument
        (or "")

    Returns
    -------
    str
        the namespaces, seperated by '::'
    str 
        the name of the element
    """

    savetmp = compoundName
    
    compoundName = removeTemplateBracketSpaces(compoundName)
    compoundName = removeTemplateInnerSpaces(compoundName)
            
    allDefinitionElements = compoundName.split(" ")
    namespaceElements = [d for d in allDefinitionElements if "::" in d]
    
    name = compoundName
    if(len(namespaceElements) < 1):
        logging.info("extract_namespace: No namespace parts for name: " + compoundName)
    
    if len(namespaceElements) > 0:
        # This checks in which string the temporary name (name node in XML) is
        # contained and picks that one if existing
        if(temporaryName != ""):
            namesWithTemp = [d for d in namespaceElements if temporaryName in d]
            if len(namesWithTemp) < 1:
                name = namespaceElements[0]
            else:
                name = namesWithTemp[0]
        else:
            # no temporary sting given, best choice might be the first one:
            name = namespaceElements[0]       

    name = restoreTemplateSpaces(name)
    name = addPointerSpaces(name)
    
    allNameParts = name.split("::")
        
    logging.debug("extract_namespace: %s, %s", "::".join(allNameParts[:-1]), allNameParts[-1] + "( " + \
        savetmp + " )")
    return "::".join(allNameParts[:-1]), allNameParts[-1]