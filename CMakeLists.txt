cmake_minimum_required(VERSION 3.24)

set(CMAKE_PROJECT_VERSION "0.0.0" CACHE STRING "Project Version")
project(hestia VERSION ${CMAKE_PROJECT_VERSION} LANGUAGES C CXX)
message(STATUS "Hestia Version: " ${CMAKE_PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/infra/cmake)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# This is for warnings coming from third-party libraries coming from FetchContent
set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

include(GNUInstallDirs)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

option(HESTIA_BUILD_TESTS "Build Hestia tests." OFF)
option(HESTIA_WITH_PHOBOS "Build Phobos Object Store Integration." OFF)
option(HESTIA_WITH_MOTR "Build the Cortx Motr Integration." OFF)
option(HESTIA_WITH_PROXYGEN "Build the Proxyen webserver." OFF)
option(HESTIA_WITH_S3_CLIENT "Build the S3 Client." ON)

if(${HESTIA_BUILD_TESTS})
option(HESTIA_TEST_REDIS "Test the Redis key-value store backend (requires Redis server)" OFF)
endif()

add_subdirectory(src)

include(CTest)
add_subdirectory(test)

# This skips adding these targets if we are included by another project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
option(HESTIA_BUILD_DOCUMENTATION "Build Documentation. Depends on Doxygen." OFF)
if(HESTIA_BUILD_DOCUMENTATION)
add_subdirectory(doc)
endif()

include(DoStaticAnalysis)
add_clang_format()
add_clang_tidy()
endif()

# Platform specific setting tweaks
if(APPLE)
  if(${HESTIA_WITH_PHOBOS})
    message(WARNING "Phobos is not buildable on Mac, turning off.")
  endif()
  set(HESTIA_WITH_PHOBOS OFF) # Phobos can't be built on Mac
else()
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install Prefix" FORCE)
  endif()
endif()
include(DoPackaging)
