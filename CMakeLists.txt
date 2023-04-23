cmake_minimum_required(VERSION 3.24)

project(
    hestia
    VERSION 0.0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Options for sections of Hestia project
#
option(
    HESTIA_BUILD_TESTS
    "Build Hestia tests. Depends on Catch2."
    OFF
)

option(
    HESTIA_BUILD_DOCUMENTATION
    "Build Hestia Documentation. Depends on Doxygen"
    OFF
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/infra/cmake)

include(FetchOstk)
fetch_ostk()

include(ostk/FetchDependencies)
fetch_nlohmann_json()

#
# Include subdirectories
#
add_subdirectory(src)

target_include_directories(hestia_lib INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:src>
)

if(HESTIA_BUILD_DOCUMENTATION)
    add_subdirectory(doc)
endif(HESTIA_BUILD_DOCUMENTATION)

if (HESTIA_BUILD_TESTS)
    add_subdirectory(test)
    enable_testing()
endif(HESTIA_BUILD_TESTS)

message(STATUS "Build Options:")
message(STATUS "    CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
message(STATUS "    HESTIA_BUILD_TESTS = ${HESTIA_BUILD_TESTS}")
