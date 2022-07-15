"""
Compound classes

This module provides classes for all documented C++ objects except parameters.
These classes contain references to each other where applicable.

The package consists of the following classes:
    * Compound - base class for all documented elements
    * Type - class inheriting Compound to hold namespaces, classes and structs
    * Member - class inheriting Compound to hold all other elements.
    * Group - class for holding references to members of a type
"""

import logging

import helpers.extract_namespace as extract_namespace
import helpers.settings as settings
import parser.parse_description as parse_description

import containers.parameters as parameters


# Compounds are basically everything, namespaces, classes, functions, variables...
class Compound:
    """
    Compounds are the base class for all C++ elements.

    The case class provides basic functionality, that is common for every 
    documented C++ element. Classes that inherit from Compound only extend this
    functionality and should not override it for consistency.
    Some of the attributes will always be empty, depending on the type of the 
    object

    Attributes
    ----------
    id : str
        the reference string by which this object will be referenced by others.
        this always has the form: namespace-name, which as many elements as are 
        needed for a complete reference.
    name : str
        the name of the object
    surroundingNamespace : str
        the name of all surrounding namespaces
    kind : str
        the type of the object (class, function, variable, etc)
    briefDescription : str
        the text of the brief description in Markdown format
    detailedDescription : str
        the text of the detailed description in Markdown format
    references : set(str)
        a list of all other elements referenced somewhere in the documentation
        of this object
    parameters : containers.parameters.ParameterContainer
        a list of parameters that belong to this element (only for functions)
    enumValues : containers.parameters.EnumContainer
        a list of enum values that belong to this element (only for enums)
    protectionLevel : str
        a string describing the protection level (public, protected, private)
    qualifiers : list(str)
        a list of qualifiers (const, virtual, inline, etc)

    Methods
    -------
    fill(node, compoundName)
        Fills the members of this object with the documentation info from
        the XML node
    """

    def __init__(self):
        self.id = ""
        self.name = ""
        self.surroundingNamespace = ""
        self.kind = ""
        self.briefDescription = ""
        self.detailedDescription = ""
        self.references = set()
        self.parameters = parameters.ParameterContainer()
        self.enumValues = parameters.EnumContainer()
        self.protectionLevel = ""
        self.qualifiers = []

    def fill(self, node, compoundName):
        """Fill members of this object with data

        This function will iterate through the contents of the XML node 'node' 
        and parse the documentation, filling the members of this object.

        Parameters
        ----------
        node : xml.etree.ElementTree.Element
            The XML node with the information to fill this object with
        compoundName : str
            The complete name of the object. This will include namespaces, 
            return values, const specifiers, templates, and other things as well
            as the real name - depending on the type
        """

        logging.debug("Creating compound %s", compoundName)
        
        tempName = node.findtext('name', "")
        self.surroundingNamespace, self.name = \
            extract_namespace.splitNamespacesAndNames(compoundName, tempName)

        self.kind = node.get('kind', "")

        self.protectionLevel = node.get('prot', "")

        oldrefid = node.get('id')
        self.id = settings.referenceDictionary[oldrefid]

        # descriptions
        if node.find('briefdescription'):
            self.briefDescription = parse_description.parseDescription\
                (node.find('briefdescription'), self.references, self.parameters)
        if node.find('detaileddescription'):
            self.detailedDescription = parse_description.parseDescription\
                (node.find('detaileddescription'), self.references, self.parameters)

        # node type is used for return values for functions and variable types.
        # I only want to include it once!
        if self.kind != "variable" and self.kind != "typedef":
            self.parameters.addTypes(node, self.references)
        if self.kind == "enum":
            self.enumValues.addEnumValues(node, self.references)
            
        for q in settings.qualifierAttributes:
            qualifierValue = node.get(q, "")
            if qualifierValue != "" and qualifierValue != "no" and \
                qualifierValue != "non-virtual":
                self.qualifiers.append(q)


    def __str__(self):
        basics = "name = %s\nid = %s\nkind = %s\nbrief = %s\ndetailed = %s\n" \
            % (self.name, self.id, self.kind, self.briefDescription, \
            self.detailedDescription)

        referenceString = "references = "
        for r in self.references:
            referenceString += r + ", "
        referenceString += "\n"

        paramtersString = "Paramters: " + str(self.parameters) + "\n"

        return basics + referenceString + paramtersString


class Type(Compound):
    """
    Container class for namespace, class and struct objects

    Inherits from Compound and extends it by adding Groups for inner elements

    Attributes
    ----------
    innerGroups : list(containers.compounds.Group)
        a list of references to other objects that belong to this type 
        (meaning internal classes, functions, variables, etc)
    base : list(str)
        if this type has base classes from which it inherits, the references
        are listed here
    derived : list(str)
        if this type has derived classes that inherit this type, the references
        are listed here
    title : str
        if this type has a title node it is stored in title

    Methods
    -------
    addInnerTypes(nodes, name)
        Adds a new Group with references to other types to innerGroups
    addInheritance(direction, nodes)
        Adds a new Group with references to other types to innerGroups
    fill(node)
        Fills all Members of this object with data from the node.
        This calls the fill function of super class Compound.
    """

    def __init__(self):
        super(Type, self).__init__()
        self.innerGroups = []

    def addInnerTypes(self, nodes, name):
        """Add a group for inner types

        This function creates a new containers.compounds.Group, adds it to the
        list innerGroup and fills it with the references in this group.

        Parameters
        ----------
        nodes : xml.etree.ElementTree.Element
            list of all inner nodes that belong to this group
        name : str
            name of the group (usually "Classes", "Namespaces", etc)
        """

        if len(nodes) > 0:
            innerGroup = Group(name)
            for inner in nodes:
                innerReference = settings.referenceDictionary[inner.get('refid')]
                innerGroup.addMember(innerReference)
            self.innerGroups.append(innerGroup)
            
    def addInheritance(self, direction, nodes):
        """Add inheritance information

        This function creates new lists for 
            * base classes (classes from which this type inherits)
            * derived classes (classes that inherit this type)

        Parameters
        ----------
        direction : str
            either "derived" or "base"
        nodes : xml.etree.ElementTree.Element
            compound reference nodes which references should be added
        """

        if direction == "derived":
            self.derived = []
            inheritanceObject = self.derived
        else:
            self.base = []
            inheritanceObject = self.base
        
        for inner in nodes:
            text = parse_description.parseReference(inner, "", self.references)
            inheritanceObject.append(text)
            
    def fill(self, node):
        """Fill members of this object with data

        This function will iterate through the contents of the XML node 'node' 
        and parse the documentation, filling the members of this object.
        It calls the fill function of the super class with the 'compoundname' 
        and fills inner groups and inheritance if applicable.

        Parameters
        ----------
        node : xml.etree.ElementTree.Element
            The XML node with the information to fill this object with
        """
        
        compoundName = node.findtext('compoundname')
        super().fill(node, compoundName)

        titleNode = node.find('title')
        if titleNode is not None:
            self.title = titleNode.text

        # get references from inner classes/namespaces (/structs = class here)
        innerTypeNodes = node.findall('innerclass')
        self.addInnerTypes(innerTypeNodes, 'Classes')

        innerTypeNodes = node.findall('innernamespace')
        self.addInnerTypes(innerTypeNodes, 'Namespaces')
        
        inheritanceNode = node.findall('derivedcompoundref')
        if inheritanceNode:
            self.addInheritance("derived", inheritanceNode)
            
        inheritanceNode = node.findall('basecompoundref')
        if inheritanceNode:
            self.addInheritance("base", inheritanceNode)


    def __str__(self):
        superString = super().__str__()
        groupString = "groups: "
        for group in self.innerGroups:
            groupString += str(group) + "\n"

        return superString + groupString


# Members are all Compounds, that are not Types (that is member variables, functions, typedefs, enums...)
# every Member is indirectly part of a Type by using Groups (see below)
class Member(Compound):
    """
    Container class for all objects that are not Types

    This container will hold member variables, functions, typedefs, enums...
    every Member is indirectly part of a containers.compounds.Type by using 
    containers.compunds.Groups.

    Attributes
    ----------
    arguments : str
        the string will all the arguments, as it was written in the source code
    initializer : str
        the initializer string (usually some "= default" or "= 0")
    variableType : str
        the type of the variable or the return value of a function

    Methods
    -------
    fill(node)
        Fills all Members of this object with data from the node.
        This calls the fill function of super class Compound.
    """

    def __init__(self):
        super(Member, self).__init__()
        # self.innerType = ""
        self.arguments = ""
        self.initializer = ""
        self.variableType = ""

    def fill(self, node):
        """Fill members of this object with data

        This function will iterate through the contents of the XML node 'node' 
        and parse the documentation, filling the members of this object.
        It calls the fill function of the super class with the 'definition' or
        'name'.

        Parameters
        ----------
        node : xml.etree.ElementTree.Element
            The XML node with the information to fill this object with
        """

        memberName = node.findtext('definition', "")
        if node.get("kind", "") == "enum":
            memberName = node.findtext('name', "")
        super().fill(node, memberName)

        type = ""
        if(node.find('type') is not None):
            type = parse_description.parseParaRecursive\
                (node.find('type'), type, self.references, None)
        else:
            logging.warning("Member.fill: Node 'type' not found in " + memberName)
            
        self.variableType = type
        if self.kind == "function" and type != "":
            self.parameters.setReturnParameter(type)
        
        self.arguments = node.findtext('argsstring')
        self.initializer = node.findtext('initializer')


    def __str__(self):
        superString = super().__str__()

        returnString = "arguments = %s\ninitializer = %s\nvariableType = %s\n" \
            % (self.arguments, self.initializer, self.variableType)

        return superString + returnString


# doxygen groupings of various kinds
# Bridge between Types and their Members
class Group:
    """
    Container class for groupings of various kinds

    This container is mostly used to link Types to Members. So every Type has 
    one or more Group objects holding references to the Members that belong to
    it.

    Attributes
    ----------
    name : str
        the name of the Group
    innerMemberRefs : list(str)
        a list of references to the members in this group

    Methods
    -------
    addMember(reference)
        Add the reference to this Group
    """

    def __init__(self, newName):
        self.name = newName
        self.innerMemberRefs = []

    def addMember(self, reference):
        """Add a member to this group

        Parameters
        ----------
        reference : str
            The reference to add
        """
        self.innerMemberRefs.append(reference)

    def __str__(self):
        returnString = "groupname:%s \n" % self.name
        for member in self.innerMemberRefs:
            returnString += str(member) + "\n"

        return returnString
