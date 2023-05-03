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
        GIT_TAG af09fd65fcf24eee95dc62813ba9123414635428 # v3.2.1
        SYSTEM
    )

    #FetchContent_Declare(sodium
    #    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    #    GIT_TAG 99f14233eab1d4f7f49c2af4ec836f2e701c445e # HEAD as of 2022-05-28
    #)
    #FetchContent_MakeAvailable(sodium)
    #find_package(sodium REQUIRED)
    #set(LIBSODIUM_INCLUDE_DIR ${sodium_SOURCE_DIR}>)

    find_package(OpenSSL REQUIRED)

    FetchContent_Declare(
    folly
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY https://github.com/facebook/folly.git
    GIT_TAG 1c74accaed8e224a7867404822ab58acb70b5c8a # v2023.02.13.00
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
    fizz
    OVERRIDE_FIND_PACKAGE
    GIT_REPOSITORY https://github.com/facebookincubator/fizz.git
    GIT_TAG e80100d2ab584a59555ed15fd78fdebb241db560 # v2023.02.13.00
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
    GIT_TAG 16bd05495c407a1e07934f9cff60517e3047512d # v2023.02.13.00
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
    GIT_TAG 54b8c0dceb6fed4e794192667ccaef257ba8005b # v2023.02.13.00
    SYSTEM
    )
    FetchContent_MakeAvailable(proxygen)

    FetchContent_Declare(
        zstd
        OVERRIDE_FIND_PACKAGE
        GIT_REPOSITORY https://github.com/facebook/zstd.git
        GIT_TAG 945f27758c0fd67b636103a38dbf050266c6b90a # v1.5.4
        SOURCE_SUBDIR build/cmake
        )
    
    FetchContent_MakeAvailable(zstd)

endmacro()