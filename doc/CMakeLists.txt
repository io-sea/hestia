find_package(Doxygen REQUIRED)

set(DOXYGEN_WARN_LOGFILE ${CMAKE_BINARY_DIR}/doxygen_warnings.txt)
set(DOXYGEN_QUIET TRUE)

doxygen_add_docs(
    doxygen
    ${PROJECT_SOURCE_DIR}/src
)

