cmake_minimum_required(VERSION 3.0)

project(
    hestia
    VERSION 0.0.1 
    LANGUAGES C CXX 
)

#
# Options for sections of Hestia project
#
option(
    HESTIA_BUILD_TESTS
    "Build Hestia tests. Depends on Catch2."
    OFF
)

#
# Include subdirectories
#

add_subdirectory(src)

if (HESTIA_BUILD_TESTS)
    add_subdirectory(test)
    enable_testing()
endif(HESTIA_BUILD_TESTS)

message(STATUS "Build Options:")
message(STATUS "    CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "    HESTIA_BUILD_TESTS = ${HESTIA_BUILD_TESTS}")
