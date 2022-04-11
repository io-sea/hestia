find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

#
# Set test files to be added to the test executable
#
#set(
#    tests
#
#    test1.cc
#    test2.cc
#)

set(sources catch_main.cc ${tests})

add_executable(all_tests ${sources})
target_link_libraries(all_tests PRIVATE Catch2::Catch2)
catch_discover_tests(all_tests)
