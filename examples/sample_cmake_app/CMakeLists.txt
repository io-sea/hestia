cmake_minimum_required(VERSION 3.5)

project(hestia_sample_app VERSION 0.0.1 LANGUAGES C CXX)

find_package(hestia REQUIRED)

option(WITH_CPP_APP "Build the c++ app" ON)
if (WITH_CPP_APP)
    set(CMAKE_CXX_STANDARD 17)
    add_executable(hestia_sample_app_cpp main.cc)
    target_link_libraries(hestia_sample_app_cpp PRIVATE hestia::hestia_lib)
endif()

add_executable(hestia_sample_app_c main.c)
target_link_libraries(hestia_sample_app_c PRIVATE hestia::hestia_lib)

add_executable(hestia_sample_iosea_app_c main_iosea.c)
target_link_libraries(hestia_sample_iosea_app_c PRIVATE hestia::hestia_lib)