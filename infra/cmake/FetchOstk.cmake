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
macro(fetch_ostk)
    hestia_fetch_openssl()

    set(OSTK_WITH_PHOBOS OFF CACHE INTERNAL "")
    set(OSTK_WITH_S3_CLIENT OFF CACHE INTERNAL "")
    set(OSTK_BUILD_TESTS OFF CACHE INTERNAL "")
    
    FetchContent_Declare(
    ostk
    GIT_REPOSITORY https://git.ichec.ie/io-sea-internal/objectstore-toolkit
    GIT_TAG        7cbd972776668623f5196555d2569c877cf7fedb
    SYSTEM
    FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(ostk)
    if(NOT TARGET ostk::ostk)
        add_library(ostk::ostk ALIAS ostk)
    endif()

    if(TARGET ostk::ostk_cmake_modules)
        get_property(OSTK_MODULES_DIR TARGET ostk::ostk_cmake_modules PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    else()
        set(OSTK_MODULES_DIR ${ostk_SOURCE_DIR}/infra/cmake)
    endif()
    list(APPEND CMAKE_MODULE_PATH ${OSTK_MODULES_DIR})
endmacro()