
def isMemberAType(object):
    return object["kind"] == "namespace" or object["kind"] == "class" \
        or object["kind"] == "struct"
        
    
def hasGroupOnlyTypes(allObjects, group):
    for reference in group["innerMemberRefs"]:
        newObject = allObjects[reference]
        
        if not isMemberAType(newObject):
            return False
    
    return True
    
def hasAnyParameterDescription(parameterList):
    for p in parameterList:
        if p["description"] != "":
            return True
    return False

def hasDescription(object):
    if object["briefDescription"] != "":
        return True
    if object["detailedDescription"] != "":
        return True
    parameters = object["parameters"]
    if len(parameters["templateParameters"]) > 0 and \
        hasAnyParameterDescription(parameters["templateParameters"]):
        return True
    if len(parameters["parameters"]) > 0 and \
        hasAnyParameterDescription(parameters["parameters"]):
        return True
    if len(object["enumValues"]["values"]) > 0 and \
        hasAnyParameterDescription(object["enumValues"]["values"]):
        return True
    if parameters["returnParameter"] is not None and \
        parameters["returnParameter"]["decltype"] != "void" and \
        parameters["returnParameter"]["description"]:
        return True

    return False
    
def hasGroupAnyDescription(allObjects, group):
    for reference in group["innerMemberRefs"]:
        newObject = allObjects[reference]
        
        if hasDescription(newObject):
            return True
    
    return False
    
def hasParameterDirection(parameters):
    if len(parameters) > 0:
        for p in parameters:
            if p["parametergroup"] != "":
                return True
    return False