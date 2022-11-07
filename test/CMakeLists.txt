find_package(Catch2 REQUIRED)

include(CTest)
include(Catch)

#
# Set test files to be added to the test executable
#
set(
    tests
        
    put_get.cc
    set_get_attrs.cc
    list_attrs.cc
    remove.cc
    list.cc
    locate.cc
    get_tiers_info.cc
    list_tiers.cc
    move.cc
    release.cc
    copy.cc
)

set(sources catch_main.cc ${tests})

add_executable(all_tests ${sources})
target_link_libraries(all_tests PRIVATE hestia_lib Catch2::Catch2)
catch_discover_tests(all_tests)
