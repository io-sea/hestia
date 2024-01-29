include(FetchContent)
include(ExternalProject)

# Avoid timestamp warnings for URL downloads
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

# https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md
macro(fetch_catch2)
    FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        05e10dfccc28c7f973727c54f850237d07d5e10f # v3.5.2
    SYSTEM
    FIND_PACKAGE_ARGS
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(Catch)
endmacro()

# https://json.nlohmann.me/integration/cmake/#supporting-both
macro(fetch_nlohmann_json)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        SYSTEM
        FIND_PACKAGE_ARGS 3.11
        )
        FetchContent_MakeAvailable(json)
endmacro()

# https://github.com/gabime/spdlog
macro(fetch_spdlog)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        7c02e204c92545f869e2f04edaab1f19fe8b19fd # v1.13.0
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(spdlog)
        if(CMAKE_BUILD_TYPE MATCHES Debug AND HESTIA_ENABLE_SANITIZERS)
            target_link_libraries(spdlog PRIVATE sanitizer_flags)
        endif()    
endmacro()

# https://github.com/jbeder/yaml-cpp
# Note: system versions of yaml-cpp < 0.7 have a broken cmake config
# If they are installed find_package will break - so for now at least
# always force fetching from remote.
macro(fetch_yaml_cpp)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
            GIT_TAG  f7320141120f720aecc4c32be25586e7da9eb978 # v0.8
            SYSTEM
            OVERRIDE_FIND_PACKAGE
            )
        set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "")
        set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
        set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
        FetchContent_MakeAvailable(yaml-cpp)
        if(CMAKE_BUILD_TYPE MATCHES Debug AND HESTIA_ENABLE_SANITIZERS)
            target_link_libraries(yaml-cpp PRIVATE sanitizer_flags)
        endif()      
endmacro()

# https://github.com/madler/zlib
macro(fetch_zlib)
    FetchContent_Declare(
        ZLIB
        GIT_REPOSITORY https://github.com/madler/zlib
        GIT_TAG        51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf # v1.3.1
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(ZLIB)

        if(NOT TARGET ZLIB::ZLIB)
            add_library(ZLIB::ZLIB STATIC IMPORTED GLOBAL)
            set_property(TARGET ZLIB::ZLIB PROPERTY IMPORTED_LOCATION ${zlib_BINARY_DIR}/libz.a)
            set_property(TARGET ZLIB::ZLIB PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
            add_dependencies(ZLIB::ZLIB zlibstatic)
            set(ZLIB_LIBRARY ${zlib_BINARY_DIR}/libz.a CACHE INTERNAL "")
        endif()     
endmacro()

# https://gitlab.gnome.org/GNOME/libxml2
macro(fetch_libxml2)
    FetchContent_Declare(
        LibXml2
        GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libxml2
        GIT_TAG        8292f361458fcffe0bff515a385be02e9d35582c # v2.12.4
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        set(LIBXML2_WITH_LZMA OFF CACHE INTERNAL "")
        set(LIBXML2_WITH_ZLIB OFF CACHE INTERNAL "")
        set(LIBXML2_WITH_PYTHON OFF CACHE INTERNAL "")
        set(LIBXML2_WITH_PROGRAMS OFF CACHE INTERNAL "")
        set(LIBXML2_WITH_TESTS OFF CACHE INTERNAL "")
        FetchContent_GetProperties(LibXml2)
        if(NOT libxml2_POPULATED)
            find_package(LibXml2 QUIET)
            if (LibXml2_FOUND)
                set(libxml2_POPULATED TRUE)
            else()
                FetchContent_Populate(LibXml2)
                add_subdirectory(${libxml2_SOURCE_DIR} ${libxml2_BINARY_DIR} EXCLUDE_FROM_ALL SYSTEM)
            endif()
        endif()
endmacro()

macro(fetch_curl)
    FetchContent_Declare(
        CURL
        GIT_REPOSITORY https://github.com/curl/curl/
        GIT_TAG 7161cb17c01dcff1dc5bf89a18437d9d729f1ecd # V8.5.0
        SYSTEM
        FIND_PACKAGE_ARGS
    )
    set(CURL_ENABLE_SSL OFF CACHE INTERNAL "")
    set(BUILD_CURL_EXE OFF CACHE INTERNAL "")
    set(HTTP_ONLY ON CACHE INTERNAL "")
    FetchContent_GetProperties(CURL)
    if(NOT curl_POPULATED)
        find_package(CURL QUIET)
        if (CURL_FOUND)
            set(curl_POPULATED TRUE)
        else()
            FetchContent_Populate(CURL)
            add_subdirectory(${curl_SOURCE_DIR} ${curl_BINARY_DIR} EXCLUDE_FROM_ALL SYSTEM)
        endif()
    endif()
endmacro()

# https://github.com/redis/hiredis
macro(fetch_hiredis)
    FetchContent_Declare(
        hiredis
        GIT_REPOSITORY https://github.com/redis/hiredis
        GIT_TAG        60e5075d4ac77424809f855ba3e398df7aacefe8 # v1.2.0
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        set(DISABLE_TESTS ON CACHE INTERNAL "disable hiredis tests")
        FetchContent_GetProperties(hiredis)
        if(NOT hiredis_POPULATED)
            FetchContent_Populate(hiredis)
            add_subdirectory(${hiredis_SOURCE_DIR} ${hiredis_BINARY_DIR} EXCLUDE_FROM_ALL SYSTEM)
        endif()
endmacro()

macro(fetch_sqlite)
    FetchContent_Declare(sqlite
    URL https://www.sqlite.org/2024/sqlite-amalgamation-3450000.zip
    URL_HASH          SHA256=bde30d13ebdf84926ddd5e8b6df145be03a577a48fd075a087a5dd815bcdf740
    SYSTEM
    FIND_PACKAGE_ARGS
    )
    if(NOT sqlite_POPULATED)
        FetchContent_Populate(sqlite)
    endif()
endmacro()

macro(build_openssl)
    ExternalProject_Add(openssl
    URL https://www.openssl.org/source/openssl-1.1.1w.tar.gz
    URL_HASH          SHA256=cf3098950cb4d853ad95c0841f1f9c6d3dc102dccfcacd521d93925208b76ac8
    CONFIGURE_COMMAND <SOURCE_DIR>/config --prefix=<INSTALL_DIR> --libdir=lib
    INSTALL_COMMAND make install_sw
    )

    set(OPENSSL_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix)
    file(MAKE_DIRECTORY ${OPENSSL_INSTALL_DIR}/include)
    add_library(OpenSSL::SSL STATIC IMPORTED GLOBAL)
    set_property(TARGET OpenSSL::SSL PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libssl.a)
    set_property(TARGET OpenSSL::SSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${OPENSSL_INSTALL_DIR}/include>)
    add_dependencies(OpenSSL::SSL openssl)

    add_library(OpenSSL::Crypto STATIC IMPORTED GLOBAL)
    set_property(TARGET OpenSSL::Crypto PROPERTY IMPORTED_LOCATION ${OPENSSL_INSTALL_DIR}/lib/libcrypto.a)
    set_property(TARGET OpenSSL::Crypto PROPERTY INTERFACE_INCLUDE_DIRECTORIES $<BUILD_INTERFACE:${OPENSSL_INSTALL_DIR}/include>)
    add_dependencies(OpenSSL::Crypto openssl)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_INSTALL_DIR}/lib/libcrypto.a CACHE INTERNAL "")
    set(OPENSSL_INCLUDE_DIR $<BUILD_INTERFACE:${OPENSSL_INSTALL_DIR}/include> CACHE INTERNAL "")
    set(OPENSSL_ROOT_DIR ${OPENSSL_INSTALL_DIR} CACHE INTERNAL "")
endmacro()

macro(fetch_openssl)
    find_package(OpenSSL 1.1.1 QUIET)
    if (NOT ${OpenSSL_FOUND})
        if(APPLE)
            set(OPENSSL_ROOT_DIR /opt/homebrew/opt/openssl@1.1)
            find_package(OpenSSL 1.1.1 QUIET)
        endif()
    endif()
    if (NOT ${OpenSSL_FOUND})
        build_openssl()
    endif()
endmacro()

macro(build_phobos)
    ExternalProject_Add(phobos
    GIT_REPOSITORY https://github.com/cea-hpc/phobos
    GIT_TAG 01822635b2a9578d36cf8733c7e58f09c15b5b0a # v1.95.1
    UPDATE_DISCONNECTED 1
    BUILD_IN_SOURCE 1
    PATCH_COMMAND   patch -p1 --forward < ${PROJECT_SOURCE_DIR}/infra/cmake/patches/phobos_ldm_common_page_size.patch 
                    || patch -p1 --forward < ${PROJECT_SOURCE_DIR}/infra/cmake/patches/phobos_disable_systemd_install.patch
                    || patch -p1 --forward < ${PROJECT_SOURCE_DIR}/infra/cmake/patches/phobos_fix_db_conn.patch            
                    || true
    CONFIGURE_COMMAND <SOURCE_DIR>/autogen.sh COMMAND CFLAGS=-Wno-error <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --disable-dependency-tracking
    BUILD_COMMAND CFLAGS=-I/usr/lib/x86_64-linux-gnu/glib-2.0/include/ make
    )

    set(PHOBOS_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/phobos-prefix)
    file(MAKE_DIRECTORY ${PHOBOS_INSTALL_DIR}/include)
    add_library(Phobos::Store STATIC IMPORTED GLOBAL)
    set_property(TARGET Phobos::Store PROPERTY IMPORTED_LOCATION ${PHOBOS_INSTALL_DIR}/lib/libphobos_store.so)
    set_property(TARGET Phobos::Store PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PHOBOS_INSTALL_DIR}/include)
    add_dependencies(Phobos::Store phobos)
endmacro()