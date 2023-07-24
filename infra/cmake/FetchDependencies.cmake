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
    GIT_TAG        3f0283de7a9c43200033da996ff9093be3ac84dc # v3.3.2
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
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
        SYSTEM
        FIND_PACKAGE_ARGS 3.11
        )
        FetchContent_MakeAvailable(json)
endmacro()

# https://github.com/gabime/spdlog
#set(SPDLOG_BUILD_SHARED ON CACHE INTERNAL "")
macro(fetch_spdlog)
    FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        ad0e89cbfb4d0c1ce4d097e134eb7be67baebb36 # v1.11.0
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(spdlog)
endmacro()

# https://github.com/jbeder/yaml-cpp
# Note: system versions of yaml-cpp < 0.7 have a broken cmake config
# If they are installed this will break
macro(fetch_yaml_cpp)
    FetchContent_Declare(
            yaml-cpp
            GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
            GIT_TAG  0579ae3d976091d7d664aa9d2527e0d0cff25763
            SYSTEM
            FIND_PACKAGE_ARGS
            )
        set(YAML_CPP_BUILD_TESTS OFF CACHE INTERNAL "")
        set(YAML_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
        set(YAML_CPP_BUILD_TOOLS OFF CACHE INTERNAL "")
        FetchContent_MakeAvailable(yaml-cpp)
endmacro()

# https://gitlab.gnome.org/GNOME/libxml2
macro(fetch_libxml2)
    FetchContent_Declare(
        LibXml2
        GIT_REPOSITORY https://gitlab.gnome.org/GNOME/libxml2
        GIT_TAG        223cb03a5d27b1b2393b266a8657443d046139d6 # v2.10.4
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(LibXml2)
endmacro()

# https://github.com/redis/hiredis
macro(fetch_hiredis)
    FetchContent_Declare(
        hiredis
        GIT_REPOSITORY https://github.com/redis/hiredis
        GIT_TAG        c14775b4e48334e0262c9f168887578f4a368b5d # v1.1.0
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

# https://github.com/bji/libs3
set(LIBS3_PATCH_COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/infra/cmake/libs3_wrapper/ <SOURCE_DIR>)
macro(fetch_libs3)
    FetchContent_Declare(
        libs3
        GIT_REPOSITORY https://github.com/bji/libs3
        GIT_TAG        287e4bee6fd430ffb52604049de80a27a77ff6b4 # master
        PATCH_COMMAND ${LIBS3_PATCH_COMMAND}
        SYSTEM
        FIND_PACKAGE_ARGS
        )
        FetchContent_GetProperties(libs3)
        if(NOT libs3_POPULATED)
            FetchContent_Populate(libs3)
            add_subdirectory(${libs3_SOURCE_DIR} ${libs3_BINARY_DIR} EXCLUDE_FROM_ALL SYSTEM)
        endif()
        if(NOT TARGET libs3::s3)
            add_library(libs3::s3 ALIAS s3)
        endif()
endmacro()

macro(build_openssl)
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
    GIT_TAG 2c32a8d7a56b14312ad0fb327d77f4b7430a27b7
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