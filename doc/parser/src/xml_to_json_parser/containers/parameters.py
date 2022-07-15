"""
Parameter classes

This module provides classes for parameters and their containers.

The package consists of the following classes:
    * Parameter - class representing one parameter
    * ParameterContainer - container for Parameter objects
    * EnumContainer - container for Parameter objects for enums
"""

import sys
import xml.etree.ElementTree as ET
import logging

import helpers.settings as settings
import parser.parse_description as parse_description


class Parameter:
    """
    Container class for the information of one parameter.

    These parameters are used in containers for classes and functions, but also
    enums (even though they are not really parameters, they are also not really 
    members, but have very similar properties to parameters and need a place 
    to be)

    Attributes
    ----------
    parametergroup : str
        a string defining the group of parameters (template, in, out, enum)
    name : str
        the name of the parameter (usually not set for templates)
    decltype : str
        type of the parameter - only set for "normal" function parameters
    description : str
        the description of this parameter
    defaultValue : str
        the default value of the parameter (only set if existant and not set for
        templates)
    reference : str
        the reference of the parameter (only set for enum values)

    Methods
    -------
    parseParameterItemNodes(itemNode, references)
        Parse description of a parameter node
    parseSingleEnumValue(enumValueNode, references)
        Parse description of an enum value
    """

    def __init__(self, name = "", description = "", group = "", defaultValue = ""):
        """
        Parameters
        ----------
        name : str, optional
            the name of the parameter (usually not set for templates)
        description : str, optional
            the description of this parameter
        group : str, optional
            a string defining the group of parameters (template, in, out, enum)
        defaultValue : str, optional
            the default value of the parameter (only set if existant and not set for
            templates)
        """

        self.parametergroup = group # = template | in | out | enum
        self.name = name  # only set for in, out and enum
        self.decltype = "" # set for in, out - aka normal parameters
        self.description = description
        self.defaultValue = defaultValue # only set for in and out, and only in case there is one and for enums
        self.reference = "" # only set for enum values


    def parseParameterItemNodes(self, itemNode, references):
        """Parse description of a parameter node

        This functions fills the corresponding members of this object with the 
        information found in the node

        Parameters
        ----------
        itemNode : xml.etree.ElementTree.Element
            A node of type <parameteritem>, with the parameter information
        references : str
            current list of references to be updated in case one is found in the
            description

        Returns
        -------
        str
            the name of the parameter
        str
            the description of this parameter
        """

        nameListNode = itemNode.find("parameternamelist")
        if nameListNode is not None:
            nameNode = nameListNode.find("parametername") # hoping we will never really have a list there...
            if nameNode is not None:
                self.name = nameNode.text
                self.parametergroup = nameNode.get("direction", "")

        descrNode = itemNode.find("parameterdescription")
        if descrNode is not None:
            self.description = parse_description.parseDescription \
                (descrNode, references, None)
                
        return self.name, self.description
                
    def parseSingleEnumValue(self, enumValueNode, references):
        """Parse description of an enum value

        This functions fills the corresponding members of this object with the 
        information found in the node

        Parameters
        ----------
        enumValueNode : xml.etree.ElementTree.Element
            A node of type <enumvalue>, with the documentation
        references : str
            current list of references to be updated in case one is found in the
            description
        """

        oldRef = enumValueNode.get("id")
        self.reference = settings.referenceDictionary[oldRef]

        self.name = enumValueNode.findtext("name", "")
        self.defaultValue = enumValueNode.findtext("initializer", "")
        self.description = parse_description.parseDescription \
            (enumValueNode.find("briefdescription"), references, None)
        self.description += parse_description.parseDescription \
            (enumValueNode.find("detaileddescription"), references, None)
        
        self.parametergroup = "enum"


    def __str__(self):
        str = f"p: group = {self.parametergroup}; name = {self.name};" + \
            f"type = {self.decltype}; descr = {self.description}; " + \
            f"defaultValue= {self.defaultValue}; reference= {self.reference}" #\
            # % (self.parametergroup, self.name, self.decltype, \
            # self.description, self.defaultValue, self.reference)
        return str

class ParameterContainer:
    """
    Container class a set of parameters

    This object will be part of containers.compounds.Members or 
    containers.compounds.Types, to which these parameters belong to.
    It consists of three lists, because in theory the Member might have all 
    three kinds of parameters, see attributes below.

    Attributes
    ----------
    parameters : list(containers.compounds.Parameter)
        a list of parameters (usually the function argument kind)
    templateParameters : list(containers.compounds.Parameter)
        a list of template parameters
    returnParameter : containers.compounds.Parameter
        a parameter representing the return value

    Methods
    -------
    parseParameterList(parameterListNode, references)
        Parse a parameter list and fill lists
    setTypeOfParam(type, parameterName, defaultValue, list, addIfNotFound)
        Set the type information of a parameter
    getElementFromParamNode(identifier, paramNode, references)
        Get inner text from a param node
    addTypesToParams(memberNode, references)
        Add types to normal parameters
    addTypesToTemplates(memberNode, references)
        Add types to template parameters
    addTypes(memberNode, references)
        Add types to all parameters
    setReturnParameter(type)
        Set the return parameter type
    addReturnDescription(description)
        Set the return parameter description
    """

    def __init__(self):
        self.parameters = []
        self.templateParameters = []
        self.returnParameter = None
    
    def parseParameterList(self, parameterListNode, references):
        """Parse a parameter list

        Iterates through the list and fills internal lists with new parameters

        Parameters
        ----------
        parameterListNode : xml.etree.ElementTree.Element
            A node of type <parameterlist>, with inner parameter nodes
        references : str
            current list of references to be updated in case one is found in the
            description

        Returns
        -------
        str
            If the node did not contain parameters (this is the case for 
            exceptions, retvals and similar things, because Doxygen is chaos), 
            this string contains the markdown for these special elements.
        """

        notAParameterDescription = ""
        kind = parameterListNode.get("kind")
        for item in parameterListNode.findall('parameteritem'):
            newParam = Parameter()
            name, description = newParam.parseParameterItemNodes(item, references)
            
            if kind == "templateparam":
                self.templateParameters.append(newParam)
            elif kind == "param":
                self.parameters.append(newParam)
            else: # It is a bit ugly to parse retval and exceptions here, because it doens't really belong here
                notAParameterDescription += "\n> **" + kind + ' ' + name +\
                    ":** " + description + "\n"
            
        return notAParameterDescription

    def setTypeOfParam(self, type, parameterName, defaultValue, list, addIfNotFound):
        """Set the type information of a parameter

        Because types of parameters are stored at different positions in the XML
        than the parameters themselves, this function searches for the correct
        parameter in the list and adds the type.

        This iterates not on the lists of the 'self' object, but on the list 
        attribute.

        Parameters
        ----------
        type : str
            the type to set to a parameter (could be the name in case of 
            template parameters, because yes, Doxygen...)
        parameterName : str
            the name of the parameter whose type shall be set
        defaultValue : str
            the default value of this parameter (or "")
        list : list(Parameter)
            the parameter list in which the parameter should be
        addIfNotFound : bool
            signalling, if the parameter should be added if it is not found 
            (again, bugs with copydoc, typedefs, templates, etc)
        """

        # search for correct parameterName in parameterList
        foundParameter = False
        for p in list:
            if parameterName == p.name:
                p.decltype = type
                p.defaultValue = defaultValue
                foundParameter = True
                break
            
            # This is because with templates, the type and name could be mixed up
            if type == p.name:
                p.decltype = parameterName
                p.defaultValue = defaultValue
                foundParameter = True
                break
        
        if not foundParameter and addIfNotFound:
            logging.info("parameters: I could not find parameter for description of " + \
                parameterName + " (" + type + ") and will now create one")
            newParam = Parameter()
            newParam.name = parameterName
            newParam.decltype = type
            newParam.defaultValue = defaultValue
            list.append(newParam)

    def getElementFromParamNode(self, identifier, paramNode, references):
        """Get inner text from a param node

        This is a helper function to extract info from a XML node that only 
        occurrs in parameter descriptions

        Parameters
        ----------
        identifier : str
            the name of the string that the function should find
        paramNode : xml.etree.ElementTree.Element
            A node of type <param>, in which we are searching
        references : str
            current list of references to be updated in case one is found in the
            description

        Returns
        -------
        str
            the string of the element we were searching for or ""
        """

        element = ""
        elementNode = paramNode.find(identifier)
        if elementNode is not None:
            # I know its not a para node, but it should still work 
            # executing only a small subset of that function
            element = parse_description.parseParaRecursive\
                (elementNode, element, references, None) 
        return element


    def addTypesToParams(self, memberNode, references):
        """Add types to the normal parameters

        This function iterates over param nodes in the memberNode and uses 
        getElementFromParamNode and setTypeOfParam to set all types.
        This is only called for "normal" parameters, not templates or return 
        values

        Parameters
        ----------
        memberNode : xml.etree.ElementTree.Element
            A node of type <memberdef>, that contains the <param> nodes
        references : str
            current list of references to be updated in case one is found in the
            description
        """

        for p in memberNode.findall('param'):
            type = self.getElementFromParamNode('type', p, references)
            
            name = self.getElementFromParamNode('declname', p, references)
                
            defaultValue = self.getElementFromParamNode('defval', p, references)
            
            if name != "":
                self.setTypeOfParam(type, name, defaultValue, self.parameters, True)
                
                
    def addTypesToTemplates(self, memberNode, references):
        """Add types to the template parameters

        This function iterates over param nodes in the memberNode and uses 
        getElementFromParamNode and setTypeOfParam to set all types.
        This is only called for template parameters, not normal ones or return 
        values

        Parameters
        ----------
        memberNode : xml.etree.ElementTree.Element
            A node of type <templateparamlist>, that contains the <param> nodes
        references : str
            current list of references to be updated in case one is found in the
            description
        """

        for p in memberNode.findall('param'):
            type = self.getElementFromParamNode('type', p, references)
            if not type:
                return
            templateSubParts = type.split(" ")
            if len(templateSubParts) < 2:
                name = self.getElementFromParamNode('declname', p, references)
                if name != "":
                    self.setTypeOfParam(templateSubParts[0], \
                        name, "", self.templateParameters, False)
                else:
                    self.setTypeOfParam("", \
                        templateSubParts[0], "", self.templateParameters, False)
            else:
                self.setTypeOfParam(templateSubParts[0], \
                    templateSubParts[1], "", self.templateParameters, False)
                
    def addTypes(self, memberNode, references):
        """Add types to all parameters

        Searches for parameter types and calls addTypesToParams (and 
        addTypesToTemplates if applicable)

        Parameters
        ----------
        memberNode : xml.etree.ElementTree.Element
            A node of type <memberdef>, that contains the <param> nodes for normal
            parameters and maybe a <templateparamlist> for templates
        references : str
            current list of references to be updated in case one is found in the
            description
        """

        templateParamNode = memberNode.find("templateparamlist")
        if templateParamNode is not  None:
            self.addTypesToTemplates(templateParamNode, references)
            
        self.addTypesToParams(memberNode, references)
        
    def setReturnParameter(self, type):
        """Set the return parameter type

        Sets the type of the return parameter variable and creates it if it 
        doesn't exist.

        Parameters
        ----------
        type : str
            The type of the return value
        """

        if self.returnParameter is None:
            self.returnParameter = Parameter()
        self.returnParameter.parametergroup = "return"
        self.returnParameter.decltype = type
        
        
    def addReturnDescription(self, description):
        """Set the return parameter description

        Sets the description of the return parameter variable and creates it if 
        it doesn't exist.

        Parameters
        ----------
        description : str
            The description of the return value
        """

        if self.returnParameter is None:
            self.returnParameter = Parameter()
        self.returnParameter.description = description
            
        
class EnumContainer:
    """
    Container class for enum values.

    For Doxygen reasons, enum values are treated as parameters. It does make
    sense in a way, because they share a lot of the documentation fields.
    This container holds a list of the values.
    This is an own class, because the fill function has to be a little 
    different and the ParameterContainer was already very overloaded.

    Attributes
    ----------
    values : list(containers.parameters.Parameter)
        a list with the enum values

    Methods
    -------
    addEnumValues(node, references)
        Parse all values in node and add them to the values list
    """

    def __init__(self):
        self.values = []
    
    def addEnumValues(self, node, references):
        """Parse all values in node and add them to the values list

        Parameters
        ----------
        node : xml.etree.ElementTree.Element
           The <memberdef> node of the enum 
        references : str
            current list of references to be updated in case one is found in the
            description
        """

        for enumValue in node.findall('enumvalue'):
            newParam = Parameter()
            newParam.parseSingleEnumValue(enumValue, references)
            self.values.append(newParam)

