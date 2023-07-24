cmake_minimum_required(VERSION 3.0)

project(hestia_sample_app VERSION 0.0.1 LANGUAGES C CXX)

find_package(OpenSSL 1.1.1 QUIET)
if (NOT ${OpenSSL_FOUND} AND APPLE)
    set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@1.1)
endif()
find_package(hestia REQUIRED)

add_executable(hestia_sample_app_cpp main.cc)
target_link_libraries(hestia_sample_app_cpp PRIVATE hestia::hestia_lib)

add_executable(hestia_sample_app_c main.c)
target_link_libraries(hestia_sample_app_c PRIVATE hestia::hestia_lib)