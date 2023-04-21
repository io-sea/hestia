include(FetchContent)
include(ExternalProject)

# Avoid timestamp warnings for URL downloads
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

macro(fetch_nlohmann_json)
    FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
        FIND_PACKAGE_ARGS 3.11
        )
        FetchContent_MakeAvailable(json)
endmacro()

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

