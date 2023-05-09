cmake_minimum_required(VERSION 3.14)
project(motr-sample-app)

include(FindPkgConfig)
include(CTest)
find_package(Catch2 REQUIRED)
include(Catch)

set(MOTR_SRC_DIR "" CACHE PATH "Path to motr src checkout - needed for hsm includes")
pkg_check_modules(motr REQUIRED motr)

add_library(sample-app-lib SHARED 
    src/MotrInterface.h
    src/MotrInterface.cpp
    ${MOTR_SRC_DIR}/hsm/m0hsm_api.c
    )

target_include_directories(sample-app-lib PUBLIC 
    ${motr_INCLUDE_DIRS}
    ${MOTR_SRC_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(sample-app-lib PUBLIC ${motr_LINK_LIBRARIES})
target_compile_options(sample-app-lib PUBLIC ${motr_CFLAGS})

add_executable(sample-app src/main.cpp)
target_link_libraries(sample-app PRIVATE sample-app-lib)

set (UNIT_TEST_SOURCES
    test/TestMotrClient.cpp
)

add_executable(unit_tests test/unit_test_main.cpp ${UNIT_TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE sample-app-lib Catch2::Catch2 motr-ut)

catch_discover_tests(unit_tests)