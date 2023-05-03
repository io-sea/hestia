include(FetchContent)

macro(hestia_fetch_openssl)
    find_package(OpenSSL 1.1.1 QUIET)
    if (NOT ${OpenSSL_FOUND})
        if(APPLE)
            set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@1.1)
            find_package(OpenSSL 1.1.1 QUIET)
        endif()
    endif()
endmacro()

# https://git.ichec.ie/io-sea-internal/objectstore-toolkit
macro(fetch_hestia)
    hestia_fetch_openssl()

    set(hestia_WITH_PHOBOS OFF CACHE INTERNAL "")
    set(hestia_WITH_S3_CLIENT OFF CACHE INTERNAL "")
    set(hestia_BUILD_TESTS OFF CACHE INTERNAL "")
    
    FetchContent_Declare(
    hestia
    GIT_REPOSITORY https://git.ichec.ie/io-sea-internal/objectstore-toolkit
    GIT_TAG        7cbd972776668623f5196555d2569c877cf7fedb
    SYSTEM
    FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(hestia)
    if(NOT TARGET hestia::hestia)
        add_library(hestia::hestia ALIAS hestia)
    endif()

    if(TARGET hestia::hestia_cmake_modules)
        get_property(hestia_MODULES_DIR TARGET hestia::hestia_cmake_modules PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    else()
        set(hestia_MODULES_DIR ${hestia_SOURCE_DIR}/infra/cmake)
    endif()
    list(APPEND CMAKE_MODULE_PATH ${hestia_MODULES_DIR})
endmacro()