include(FetchContent)

macro(hestia_build_openssl)
    ExternalProject_Add(openssl
    URL https://www.openssl.org/source/openssl-1.1.1t.tar.gz
    URL_HASH          SHA256=8dee9b24bdb1dcbf0c3d1e9b02fb8f6bf22165e807f45adeb7c9677536859d3b
    CONFIGURE_COMMAND <SOURCE_DIR>/config --prefix=<INSTALL_DIR> --libdir=lib
    INSTALL_COMMAND make install_sw
    )

    set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix)
    file(MAKE_DIRECTORY ${OPENSSL_INSTALL_DIR}/include)
    add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
    set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libssl.a)
    set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INSTALL_DIR}/include)
    add_dependencies(OpenSSL::SSL openssl)

    add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
    set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libcrypto.a)
    set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INSTALL_DIR}/include)
    add_dependencies(OpenSSL::Crypto openssl)
endmacro()

macro(hestia_fetch_openssl)
    find_package(OpenSSL 1.1.1 QUIET)
    if (NOT ${OpenSSL_FOUND})
        if(APPLE)
            set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@1.1)
            find_package(OpenSSL 1.1.1 QUIET)
        endif()
    endif()
    if (NOT ${OpenSSL_FOUND})
        hestia_build_openssl()
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
    GIT_TAG        61bea7f6c06240d1365e6238fa783660a9c56a70
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