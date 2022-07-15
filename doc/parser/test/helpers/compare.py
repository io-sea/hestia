import os
import shutil
import re

def find_start_end_indices(membername, level, filecontents):
    regex = r"\n#{" + str(level) + r"}\ (\[|\w|<)"
    
    # matches in complete file for all headings (##) of the correct level
    headingMatches = list(re.finditer(regex, filecontents))
    
    for idx in range(len(headingMatches)):
        heading = headingMatches[idx]
        
        # find newline -> We are searching for the name only in the header line itself
        newlineRegex = r"\n"
        newlineMatch = re.search(newlineRegex, filecontents[heading.start()+1:])
        
        # is the membername in the headerline?
        memberMatch = re.search(membername, 
            filecontents[heading.start():(heading.start() + newlineMatch.start() + 1)])
            
        if memberMatch:
            # yes! return start of headerline and start of the next one (or end of file)
            if idx == len(headingMatches) - 1:
                return heading.start(), len(filecontents)
            else:
                return heading.start(), headingMatches[idx + 1].start()
    return -1, -1
                

def compare_documentation(membername, level, filename_generated, filename_reference):
    if not os.path.isfile(filename_generated) or \
        not os.path.isfile(filename_reference):
        return False, "File not found: " + filename_generated + " or " + filename_reference
    
    generated_file = open(filename_generated)
    generated = generated_file.read()
    generated_file.close()

    reference_file = open(filename_reference)
    reference = reference_file.read()
    reference_file.close()
    
    generatedIndex, generatedEnd = find_start_end_indices(membername, level, generated)
    referenceIndex, referenceEnd = find_start_end_indices(membername, level, reference)
    
    if generatedIndex == -1 or generatedEnd == -1:
        return False, "Error: Could not find " + membername + " in file " + filename_generated
    if referenceIndex == -1 or referenceEnd == -1:
        return False, "Error: Could not find " + membername + " in file " + filename_reference
    
    while(generatedIndex < generatedEnd and referenceIndex < referenceEnd):
        if generated[generatedIndex] != reference[referenceIndex]:
            return False, "Error: Documentation of member " + membername +\
                " diverges at reference index " + str(referenceIndex) + " " + \
                repr(reference[referenceIndex])
            
        generatedIndex += 1
        referenceIndex += 1

    return True, ""
    
    
def compare_fileheader(filename_generated, filename_reference):
    # TODO: Add brief and detailed description test here and and 
    # own Parameter search thing to everybody who is calling this and has parameters
    # also inner classes and namespaces! Here?
    return True, "Not implemented yet"

