import os
import shutil

def run_doxygen():
    doxygenCommand = "doxygen " + os.path.dirname(__file__) + "/../Doxyfile"
    os.system(doxygenCommand)

    currentWorkingDir = os.getcwd()
    xmlPath = currentWorkingDir + "/xml"

    if not os.path.isdir(xmlPath):
        print("Error: Doxygen did not create directory: " + xmlPath)
        return ""
    return xmlPath

def run_xml_to_json_parser(xmlPath):
    currentWorkingDir = os.getcwd()
    jsonPath = currentWorkingDir + "/output.json"
    
    executablePath = os.path.dirname(__file__) + \
        "/../../src/xml_to_json_parser/xml_to_json_parser.py"
    pythonCommand = "python3 " + executablePath + " --loglevel info " + xmlPath + " " + jsonPath
    os.system(pythonCommand)

    if not os.path.isfile(jsonPath):
        print("Error: Xml to JSON Parser did not succeed! File " + jsonPath + \
            " does not exist.")
        return ""
    return jsonPath

def run_json_to_Markdown_converter(jsonPath):
    currentWorkingDir = os.getcwd()
    # outputPath = currentWorkingDir + "/output/"
    outputPath = "output"

    executablePath = os.path.dirname(__file__) + \
        "/../../src/json_to_Markdown_converter/json_to_Markdown_converter.py"
    pythonCommand = "python3 " + executablePath + " --loglevel info " + jsonPath + " " + outputPath
    os.system(pythonCommand)

    if not os.path.isdir(outputPath):
        print("Error: JSON to Markdown converter did not succeed!")
        return ""
    return outputPath


def create_documentation():
    xmlPath = run_doxygen()
    if not xmlPath:
        return ""

    jsonFile = run_xml_to_json_parser(xmlPath)
    if not jsonFile:
        return ""

    outputFolder = run_json_to_Markdown_converter(jsonFile)
    if not outputFolder:
        return ""

    return outputFolder

def get_reference_docu_path():
    return os.path.dirname(__file__) + "/../desired_output/"
    

def cleanUp(documentationFolder):
    rootPath = os.path.abspath(os.path.join(documentationFolder, os.pardir))
    if os.path.isdir(documentationFolder):
        shutil.rmtree(documentationFolder, True)

    if os.path.isfile(rootPath + "/output.json"):
        os.remove(rootPath + "/output.json")
        
    if os.path.isfile(rootPath + "/json_to_Markdown_converter.log"):
        os.remove(rootPath + "/json_to_Markdown_converter.log")
            
    if os.path.isfile(rootPath + "/xml_to_json_parser.log"):
        os.remove(rootPath + "/xml_to_json_parser.log")

    if os.path.isdir(rootPath + "/xml"):
        shutil.rmtree(rootPath + "/xml", True)

    if os.path.isdir(rootPath + "/html"):
        shutil.rmtree(rootPath + "/html", True)
