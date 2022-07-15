"""
Module for all kinds of settings/variables used throughout the project

Global Attributes
-----------------
* pathToXML : str
    the path to the directory with the Doxygen XML files
* referenceDictionary : dict(str->str)
    dictionary mapping old references to new ones
* compoundDictionary : dict(str->containers.compounds.Compound)
    doctionary of all compund references and their objects that are found 
* kindToName : dict(str->str)
    dictionary mapping Doxygen names to nicer layout
* missingReferences : set(str)
    list of references that are linked but not setup in referenceDictionary
* qualifierAttributes : list(str)
    list of qualifier attribute names


The module consists of the following functions:
    * init 
        - initialises attributes mentioned above
    * createNewReference 
        - Creates a new reference and saves it to referenceDictionary
    * setupReferences 
        - setup all references found with new ones in referenceDictionary
"""

def init():
    """Initialise variables

    """

    global pathToXML
    pathToXML = ""

    global referenceDictionary
    referenceDictionary = dict()

    global compoundDictionary
    compoundDictionary = dict()

    global kindToName
    kindToName = dict()
    kindToName["public-type"] = "Public Types"
    kindToName["public-func"] = "Public Functions"
    kindToName["public-attrib"] = "Public Attributes"
    kindToName["protected-type"] = "Protected Types"
    kindToName["protected-func"] = "Protected Functions"
    kindToName["protected-attrib"] = "Protected Attributes"
    kindToName["private-type"] = "Private Types"
    kindToName["private-func"] = "Private Functions"
    kindToName["private-attrib"] = "Private Attributes"
    kindToName["user-defined"] = "TODO: Take Header!"
    kindToName["public-static-type"] = "Public Static Types"
    kindToName["public-static-func"] = "Public Static Functions"
    kindToName["public-static-attrib"] = "Public Static Attributes"
    kindToName["typedef"] = "Type Definitions"
    kindToName["func"] = "Functions"
    kindToName["enum"] = "Enumerations"
    kindToName["var"] = "Variables"

    global missingReferences
    missingReferences = set()
    
    global qualifierAttributes
    qualifierAttributes = ["static","strong","const","explicit", \
        "inline","refqual","virt","volatile","mutable"]

def createNewReference(oldReference, compoundName, nameDelimiter = '::'):
    """Creates a new reference and saves it to referenceDictionary

    This function takes the full name, replaces '::' with '-' and if that 
    already exists adds an increasing number to the end, until the reference is 
    unique.
    The new reference is added to the reference dictionary.

    Parameters
    ----------
    oldReference : str
        The old Doxygen reference
    compoundName : str
        The name of the compound we are dealing with
    nameDelimiter : str, optional
        The delimiter by which the name shall be split up
    """

    if oldReference in referenceDictionary:
        return

    newReference = compoundName.replace(nameDelimiter, '-')
    newReference = newReference.replace(' ', '')

    counter = 1
    nameexists = False
    for old, new in referenceDictionary.items():
        if(new == newReference):
            newReference += "-" + str(counter)
            nameexists = True
            break

    while nameexists:
        # offset = number of characters to remove from end of string: "-17" are 3
        # hint: calculated before counter is increased, since for "-9"
        #       2 have to be removed to add 3 for "-10"
        offset = int(counter / 10 ) + 2
        counter += 1
        nameexists = False
        for old, new in referenceDictionary.items():
            if(new == newReference):
                newReference = newReference[:-offset]
                newReference += "-" + str(counter)
                nameexists = True
                break


    referenceDictionary[oldReference] = newReference


def setupReferences(indexRoot):
    """Sets up all new references found in the XML

    This function iterates over the elements in indexRoot and sets up new 
    references for every element using createNewReference

    Parameters
    ----------
    indexRoot : str
        The XML root of 'index.xml', created by Doxygen
    """

    for compound in indexRoot:
        compoundKind = compound.get('kind')
            
        if(compoundKind != 'file' and \
            compoundKind != 'dir'):

            # get own reference:
            oldRef = compound.get('refid')
            compoundName = compound.findtext('name', "unnamed")
            
            if(compoundKind == 'page'):
                createNewReference(oldRef, compoundName, '_')      
                # pages don't get to have members!          
            else:
                createNewReference(oldRef, compoundName)

                for member in compound.findall('member'):
                    oldRef = member.get('refid')
                    #TODO: Attention, this is very cpp-y:
                    memberName = compoundName + "::" + member.findtext('name', "unnamed")
                    createNewReference(oldRef, memberName)


