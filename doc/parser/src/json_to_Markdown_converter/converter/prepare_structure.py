import os
import re
import logging

def checkDirectory(path):
    if not os.path.exists(path):
        os.makedirs(path)

def nameToPath(namespace, name):
    if namespace != "":
        return namespace.replace("::", "/") + '/' + name
    else:
        return name


def createFile(objectNamespace, fileName, root):
    
    namePath = nameToPath(objectNamespace, fileName)
    createPath = root + "/" + namePath + ".md"
    
    # Check if directory exists:
    directoryName = createPath.rsplit('/', 1)[0]
    checkDirectory(directoryName)
    
    open(createPath, "w+")
    return createPath


def prepare_structure(objects, root):
    namespaceFilenameDict = dict()
    
    for ref, obj in objects.items():
        if obj["kind"] is None:
            logging.error("prepare_structure: Found none kind: " + ref)
            continue
        
        if obj["kind"] == "namespace":
            completeName = obj["name"]
            if obj["surroundingNamespace"] != "":
                completeName = obj["surroundingNamespace"] + "::" + obj["name"]
            newPath = createFile(completeName, "index", root)
            obj["mdFilename"] = newPath
            namespaceFilenameDict[completeName] = newPath
        elif (obj["kind"] == "class") or (obj["kind"] == "struct"):
            newPath = \
                createFile(obj["surroundingNamespace"], obj["name"], root)
            obj["mdFilename"] = newPath
            namespaceFilenameDict[obj["surroundingNamespace"] + "::" + obj["name"]] = newPath
        elif obj["kind"] == "page":
            completeName = ""
            if(obj["name"]):
                nameSubParts = obj["name"].split('_')
                if len(nameSubParts) < 0:
                    logging.error("prepare_structure: Page with id " + obj["id"] \
                        + " does not seem to have a name")
                    continue
                elif len(nameSubParts) == 1:
                    newPath = \
                        createFile("", nameSubParts[0], root)
                    completeName = nameSubParts[0]
                else:
                    completeName = "::".join(nameSubParts)
                    newPath = \
                        createFile("::".join(nameSubParts[:-1]), nameSubParts[-1], root)
                
                obj["mdFilename"] = newPath
                namespaceFilenameDict[completeName] = newPath
                
    logging.info("namespaceFilenameDict: " + str(namespaceFilenameDict))
                
    # TODO: In doxygen, all elements are processed after their namespaces/classes,
    # therefore they only appear behind them in the json file.
    # The following could be done in the same loop then (just as else:), but this
    # is a precondition that I don't really want to assume as always given
    for ref, obj in objects.items():
        if (obj["kind"] != "namespace" and \
            obj["kind"] != "class" and \
            obj["kind"] != "struct" and \
            obj["kind"] != "page"):
            
            namespace = obj["surroundingNamespace"]
            if namespace in namespaceFilenameDict.keys():
                obj['mdFilename'] = namespaceFilenameDict[namespace]
            else:
                # template brackets and names should not be part of the namespace search
                templateRegex = r"<\s?\w+(\s?\w+)*\s?>"
                untemplatedNamespace = re.sub(templateRegex, "", namespace)
                if untemplatedNamespace in namespaceFilenameDict.keys():
                    obj['mdFilename'] = namespaceFilenameDict[untemplatedNamespace]
                else:
                    if 'mdFilename' not in obj.keys():
                        logging.warning("prepare_structure: I don't have a filename! " + \
                            obj["surroundingNamespace"] + "::" + obj["name"])

    return objects
