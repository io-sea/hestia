cmake_minimum_required(VERSION 3.0)

project(
    hestia
    VERSION 0.0.1 
    LANGUAGES C CXX 
)

add_subdirectory(src)

message(STATUS "Build Options:")
message(STATUS "    CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
