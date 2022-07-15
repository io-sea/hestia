find_package(Doxygen REQUIRED)

# Write a Doxyfile including the main doc/Doxyfile.
# This sets the @INCLUDE_PATH variable to the project root.
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    @ONLY
)

set(INCLUDE_DOXYFILE "@INCLUDE = ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

# Custom command to run doxygen on build/doc/Doxyfile in the build/doc dir.
# Running doxygen is tied to the html/index.html file.
# TODO: The main dependency is currently doc/Doxyfile, but there should be
#       a depencency on *every* file being documented!
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml
    COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building the documentation..."
)

if(CUSTOM_DOCU_PATH)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docu.json
        COMMAND python3
        ARGS "${CUSTOM_DOCU_PATH}/xml_to_json_parser/xml_to_json_parser.py" "${CMAKE_CURRENT_BINARY_DIR}/xml" docu.json
        MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Parsing the documentation XML..."
    )
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Markdown/index.md
        COMMAND python3
        ARGS "${CUSTOM_DOCU_PATH}/json_to_Markdown_converter/json_to_Markdown_converter.py" docu.json "${CMAKE_CURRENT_SOURCE_DIR}/Markdown"
        MAIN_DEPENDENCY ${CMAKE_CURRENT_BINARY_DIR}/docu.json
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Creating the Markdown documentation..."
    )
    # Target to wire up building the documentation to the ALL target.
    add_custom_target(
        doc ALL
        DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/Markdown/index.md
    )
else(CUSTOM_DOCU_PATH)
    # Target to wire up building the documentation to the ALL target.
    add_custom_target(
        doc ALL
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/xml/index.xml
    )

endif(CUSTOM_DOCU_PATH)


