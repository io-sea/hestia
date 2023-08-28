include(FetchDependencies)

macro(fetch_proxygen)
    set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL YES)

    include(FetchContent)
    FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG b796f7d44681514f58a683a3a71ff17c94edb0c1 # v1.13.0
    SYSTEM
    CMAKE_ARGS -DBUILD_GMOCK=ON -DBUILD_GTEST=ON
    OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50 # v9.1.0
    SYSTEM
    CMAKE_ARGS -DFMT_DOC=OFF -DFMT_TEST=OFF -DFMT_INSTALL=OFF
    OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        double-conversion
        GIT_REPOSITORY https://github.com/google/double-conversion.git
        GIT_TAG 4f7a25d8ced8c7cf6eee6fd09d6788eaa23c9afe # v3.3.0
        SYSTEM
    )

    #FetchContent_Declare(sodium
    #    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    #    GIT_TAG 99f14233eab1d4f7f49c2af4ec836f2e701c445e # HEAD as of 2022-05-28
    #)
    #FetchContent_MakeAvailable(sodium)
    #find_package(sodium REQUIRED)
    #set(LIBSODIUM_INCLUDE_DIR ${sodium_SOURCE_DIR}>)

    fetch_zlib()
    #fetch_openssl()
    #find_package(OpenSSL REQUIRED)

    FetchContent_Declare(
    folly
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY https://github.com/facebook/folly.git
    GIT_TAG 017e42662179411f83eb24c7100b3af7f8a61518 # v2023.07.24.00
    SYSTEM
    CMAKE_ARGS -DCMAKE_MODULE_PATH=CMake -DBUILD_TESTS=OFF -DBUILD_BENCHMARKS=OFF
    )

    set(BOOST_ENABLE_CMAKE ON)
    set(BOOST_INCLUDE_LIBRARIES math filesystem system program_options)
    FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG b6928ae5c92e21a04bbe17a558e6e066dbe632f6
    SYSTEM
    )
    FetchContent_MakeAvailable(googletest fmt double-conversion)

    #add_library(fmt::fmt ALIAS fmt)
    include(GNUInstallDirs)
    INSTALL(TARGETS fmt EXPORT fmt DESTINATION lib)
    INSTALL(EXPORT fmt DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    FetchContent_Declare(
        zstd
        OVERRIDE_FIND_PACKAGE
        GIT_REPOSITORY https://github.com/facebook/zstd.git
        GIT_TAG 63779c798237346c2b245c546c40b72a5a5913fe # v1.5.5
        SOURCE_SUBDIR build/cmake
        )
    set(ZSTD_BUILD_TESTS OFF CACHE INTERNAL "")
    set(ZSTD_BUILD_PROGRAMS OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(zstd)
    set(ZSTD_INCLUDE_DIR $<BUILD_INTERFACE:${zstd_SOURCE_DIR}/lib> CACHE INTERNAL "")
    message(STATUS "ZSTD BINARY DIR IS: " ${CMAKE_BINARY_DIR})
    set(ZSTD_LIBRARY ${CMAKE_BINARY_DIR}/lib/libzstd.a CACHE INTERNAL "")

    FetchContent_Declare(
    fizz
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY https://github.com/facebookincubator/fizz.git
    GIT_TAG 555161037025db59658ae5d0277c4c3e1e49817e # v2023.07.24.00
    SYSTEM
    SOURCE_SUBDIR fizz
    )

    FetchContent_MakeAvailable(folly)
    target_include_directories(folly_base PUBLIC $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>)
    #target_link_libraries(folly PUBLIC OpenSSL::SSL)

    FetchContent_MakeAvailable(folly fizz)

    target_include_directories(fizz PUBLIC $<BUILD_INTERFACE:${fizz_SOURCE_DIR}>)
    target_include_directories(fizz PUBLIC $<BUILD_INTERFACE:${folly_SOURCE_DIR}>)
    target_include_directories(fizz PUBLIC $<BUILD_INTERFACE:${folly_BINARY_DIR}>)
    target_include_directories(fizz PUBLIC $<BUILD_INTERFACE:${fmt_SOURCE_DIR}/include>)
    target_link_libraries(fizz PUBLIC folly)

    FetchContent_Declare(
    wangle
    GIT_REPOSITORY https://github.com/facebook/wangle.git
    GIT_TAG 68b1ec08f23196e0ad1dd2dfbb2308c095caf440 # v2023.07.24.00
    SYSTEM
    SOURCE_SUBDIR wangle
    OVERRIDE_FIND_PACKAGE
    )

    FetchContent_MakeAvailable(wangle)
    target_include_directories(wangle PUBLIC $<BUILD_INTERFACE:${wangle_SOURCE_DIR}>)
    target_include_directories(wangle PUBLIC $<BUILD_INTERFACE:${folly_SOURCE_DIR}>)
    target_include_directories(wangle PUBLIC $<BUILD_INTERFACE:${folly_BINARY_DIR}>)
    #target_link_libraries(wangle PUBLIC folly)

    target_include_directories(wangle PUBLIC $<BUILD_INTERFACE:${fizz_SOURCE_DIR}>)
    target_include_directories(wangle PUBLIC $<BUILD_INTERFACE:${fmt_SOURCE_DIR}/include>)
    target_link_libraries(wangle PUBLIC fizz)

    add_library(Folly::folly ALIAS folly)
    add_library(fizz::fizz ALIAS fizz)
    add_library(wangle::wangle ALIAS wangle)

    FetchContent_Declare(
    proxygen
    GIT_REPOSITORY https://github.com/facebook/proxygen.git
    GIT_TAG 4fbbc029ee3404e6ad94817fda8c022760354194 # v2023.07.24.00
    SYSTEM
    )
    FetchContent_MakeAvailable(proxygen)



endmacro()