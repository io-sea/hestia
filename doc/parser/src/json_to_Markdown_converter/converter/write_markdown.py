import os
import logging

import converter.content_check as content_check
import converter.settings as settings

def getRelativePath(path, referencePath):
    path = path.replace('//', '/')
    referencePath = referencePath.replace('//', '/')
    pathParts = path.split('/')
    referenceParts = referencePath.split('/')
    
    logging.debug("write_markdown: Relative path from "+ path + \
        " and " + referencePath)
    
    if len(pathParts) < 2 or len(referencePath) < 2:
        return path
    
    if path == referencePath:
        logging.debug("write_markdown: Created path " + "./" + pathParts[-1])
        return "./" + pathParts[-1]
    
    newPathPart = ""
    directoriesUpCounter = -1
    for i in range(0,max(len(pathParts), len(referenceParts))):
        if i < len(pathParts) and i < len(referenceParts):
            if pathParts[i] != referenceParts[i]:
                directoriesUpCounter += 1
                newPathPart += "/" + pathParts[i]
        elif i >= len(pathParts):
            directoriesUpCounter += 1
        elif i >= len(referenceParts):
            newPathPart += "/" + pathParts[i]
        else:
            # really, really shouldn't happen
            logging.error("write_markdown: Loop is broken! Problem!")
    
    logging.debug("write_markdown: Created " + \
        "./" + ('../' * directoriesUpCounter) + newPathPart[1:])
    return "./" + ('../' * directoriesUpCounter) + newPathPart[1:]

def removetrailingNewlines(description):
    newDescr = description
    if newDescr != "":
        while newDescr[-1] == '\n':
            newDescr = newDescr[:-1]
    return newDescr

def writeHeader(object, file, level):
    file.write(('#' * level))
    file.write(" <a name='" + object["id"] + "' />")
    
    if "protectionLevel" in object:
        file.write(' ' + object["protectionLevel"] )
    # if object["kind"] != "typedef":
    #     file.write(' ' + object["kind"])
    if object["kind"] == "function":
        file.write(' ' + object["variableType"])
    file.write(" ")
    if object["surroundingNamespace"] != "":
        file.write(object["surroundingNamespace"] + "::")
    
    if "title" in object:
        file.write(object["title"])
    else:
        file.write(object["name"])
    
    if "arguments" in object and object["arguments"] is not None:
        file.write(' ' + object["arguments"] )
        
    if "initializer" in object and object["initializer"] is not None:
        file.write(' ' + object["initializer"] )
        
def writeCompleteDescription(object, file):
    file.write("\n")
    file.write(object["briefDescription"] + '\n')
    
    # details
    if object["detailedDescription"] != ""   :
        file.write("\n")
        file.write(object["detailedDescription"] + '\n')


def writeTableHeader(file, headerlist):
    file.write("| ")
    for header in headerlist:
        file.write(header + " | ")
    file.write("\n")
    file.write("|" + (" ---- |" * len(headerlist)))
    file.write("\n")


def writeParameters(parameters, file, level):
    if len(parameters["templateParameters"]) > 0:
        file.write(('#' * level) + ' ' + "Template Parameters: " + '\n')
        writeTableHeader(file, ["Type", "Name", "Description"])
        for template in parameters["templateParameters"]:
            file.write(\
                "| " + template["decltype"] + \
                " | " + template["name"] + \
                " | " + removetrailingNewlines(template["description"]) + \
                " |\n")
        file.write('\n')


    if len(parameters["parameters"]) > 0:
        writeDirection = content_check.hasParameterDirection\
            (parameters["parameters"])
        
        file.write(('#' * level) + ' ' + "Parameters: " + '\n')
        
        if writeDirection:
            writeTableHeader(file, ["Direction", "Type", "Name", "Description" \
                # , "Default Value"\
                ])
        else:
            writeTableHeader(file, ["Type", "Name", "Description" \
                # , "Default Value"\
                ])
            
        for param in parameters["parameters"]:
            if writeDirection:
                file.write("| " + param["parametergroup"] + ' ')
            file.write(\
                "| " + param["decltype"] + \
                " | " + param["name"] + \
                " | " + removetrailingNewlines(param["description"]) + \
                # " | " + param["defaultValue"] + \
                " |\n")
        file.write('\n')
        
    if parameters["returnParameter"] is not None:
        returnValue = parameters["returnParameter"]
        if returnValue["decltype"] != "void":
            file.write(('#' * level) + ' ' + "Returns: " + '\n')
            writeTableHeader(file, ["Type", "Description"])
            file.write(\
                "| " + returnValue["decltype"] + \
                " | " + removetrailingNewlines(returnValue["description"]) + \
                " |\n"
                )
            file.write('\n')
            
def writeInheritanceInformation(currentObject, mdFile):
    if ("base" in currentObject) or ("derived" in currentObject):
        mdFile.write("## Inheritance:\n")
        inheritanceText = ""
        if ("derived" in currentObject):
            inheritanceText = "Is inherited by "
            for derivedClass in currentObject['derived']:
                inheritanceText += derivedClass + ", "
            inheritanceText = inheritanceText[:-2]
        else:
            inheritanceText = "Inherits from "
            for derivedClass in currentObject['base']:
                inheritanceText += derivedClass + ", "
            inheritanceText = inheritanceText[:-2]
        mdFile.write(inheritanceText + ".\n\n")

def writeEnums(enumValues, file, level):
    if len(enumValues["values"]) > 0:
        file.write(('#' * level) + ' ' + "Enum Values: " + '\n')
        writeTableHeader(file, ["Name", "Description", "Value"])
        for value in enumValues["values"]:
            file.write(\
                "| " + value["name"] + \
                " | " + removetrailingNewlines(value["description"]) + \
                " | " + value["defaultValue"] + \
                " |\n")
        file.write('\n')
        
def writeQualifiers(qualifiers, file, level):
    if len(qualifiers) > 0:
        file.write(('#' * level) + ' ' + "Qualifiers: " + '\n')
        for qual in qualifiers:
            if qual == "virt":
                qual = "virtual"
            file.write("* " + qual + '\n')


# def writeDetailOpenTag(file, memberName, reference):
#     file.write("<details>\n<summary> ")
#     if reference:
#         file.write("<a id='" + reference + "' /> ")
#     file.write(memberName + " </summary>\n\n")
# 
# 
# def writeDetailCloseTag(file):
#     file.write("---\n\n</details>\n\n")
    
def createFullName(object):
    fullName = object["variableType"] + " " + object["name"]
    if object["kind"] == "function" and object["arguments"] is not None:
        fullName = fullName + " " + object["arguments"]
    return fullName

def writeElement(newObject, file, level):
    # Header
    writeHeader(newObject, file, level)
    file.write("\n\n")
     
    file.write(newObject["briefDescription"] + '\n')
    file.write("\n\n")
    
    # parameters
    writeParameters(newObject["parameters"], file, level + 1)
    file.write("\n\n")
    writeEnums(newObject["enumValues"], file, level + 1)
    file.write("\n\n")
    
    # details
    if newObject["detailedDescription"] != ""   :
        file.write("\n\n")
        file.write(newObject["detailedDescription"] + '\n')
        file.write("\n\n")
        
    if len(newObject["qualifiers"]) > 0:
        writeQualifiers(newObject["qualifiers"], file, level+1)
        file.write("\n\n")
        
        
def writeGroupTable(allObjects, file, group):
    linkedReferences = []
        
    writeTableHeader(file, ["Name", "Description"])

    # write tables with brief description and links
    for reference in group["innerMemberRefs"]:
        newObject = allObjects[reference]
        if content_check.hasDescription(newObject) or settings.printUndocumented:
            
            fullName = newObject["name"]
            # if newObject["surroundingNamespace"] != "":
            #     fullName = newObject["surroundingNamespace"] + "::" + fullName
            
            # inner Type: Link to file
            if content_check.isMemberAType(newObject):
                relativePath = getRelativePath(newObject["mdFilename"], file.name)
                file.write(\
                    "| [" + fullName + "](" + relativePath + ")"\
                    " | " + removetrailingNewlines(newObject["briefDescription"]) + \
                    " |\n"
                    )
                
            # inner Member: link to same document
            else:
                file.write(\
                    "| [" + fullName + "](#" + reference + ")"\
                    " | " + removetrailingNewlines(newObject["briefDescription"]) + \
                    " |\n"
                    )
                    
            linkedReferences.extend(newObject["references"])
    
    file.write('\n')
    return allObjects, linkedReferences
    
def addGroupInformation(allObjects, currentObject, file, linkedReferences, level):
    # write tables
    for group in currentObject["innerGroups"]:
        if content_check.hasGroupAnyDescription(allObjects, group) or settings.printUndocumented:
            if group["name"]:
                file.write(('#' * level) + ' ' + group["name"] + '\n')
                
            allObjects, newRefs = writeGroupTable(allObjects, file, group)
            if newRefs:
                linkedReferences.update(newRefs)
            file.write("\n")

    file.write("\n")

    # write full member descriptions
    for group in currentObject["innerGroups"]:
        if not content_check.hasGroupOnlyTypes(allObjects, group) and \
            (content_check.hasGroupAnyDescription(allObjects, group) or settings.printUndocumented):
            if group["name"]:
                file.write(('#' * level) + ' ' + group["name"] + '\n')
                
            for reference in group["innerMemberRefs"]:
                newObject = allObjects[reference]
                
                if not content_check.isMemberAType(newObject) and\
                    (content_check.hasDescription(newObject) or settings.printUndocumented):
                    writeElement(newObject, file, level + 1)
                    file.write("[Go to Top](#" + currentObject["id"] + ")\n\n")
                
    return linkedReferences
    

def addLinks(allObjects, file, linkedReferences, enumValueDictionary):
    sortedRefs = sorted(linkedReferences)
    
    for ref in sortedRefs:
        file.write("[" + ref + "]:" )

        if ref not in allObjects.keys():
            if (enumValueDictionary is not None and \
                ref in enumValueDictionary.keys()):
                ref = enumValueDictionary[ref]
            else:
                logging.warning("write_markdown: Could not find ref " + ref \
                    + " for file " + file.name)
                file.write('\n')
                continue
        
        linkedObject = allObjects[ref]
        if "mdFilename" not in linkedObject:
            logging.warning("write_markdown: Could not find filename of " \
                + linkedObject["name"])
        else:
            relativePath = getRelativePath(linkedObject["mdFilename"], file.name)
            file.write(relativePath)

        if linkedObject["kind"] != "namespace" and linkedObject["kind"] != "class":
            file.write("#" + ref)

        file.write('\n')


def writeType(allObjects, currentObject, enumValueDictionary = None):
    if currentObject["mdFilename"] == "":
        logging.warning("write_markdown: I don't have a filename for " + \
            currentObject["surroundingNamespace"] + "::" + currentObject["name"])
        return
        
    mdFile = open(currentObject["mdFilename"], "w")

    # Header
    writeHeader(currentObject, mdFile, 1)
    mdFile.write("\n")
    writeCompleteDescription(currentObject, mdFile)
    mdFile.write("\n")

    # parameters
    writeParameters(currentObject["parameters"], mdFile, 2)
    mdFile.write("\n")

    # Add Inheritance information if existing
    writeInheritanceInformation(currentObject, mdFile)

    # groups
    linkedReferences = set(currentObject["references"])
    if "innerGroups" in currentObject:
        linkedReferences = addGroupInformation(allObjects, currentObject, mdFile, linkedReferences, 2)

    # Add links
    addLinks(allObjects, mdFile, linkedReferences, enumValueDictionary)
