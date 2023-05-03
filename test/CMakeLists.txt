add_subdirectory(base)

if(HESTIA_BUILD_TESTS)
include(FetchDependencies)
fetch_catch2()

add_subdirectory(hsm_object_store)

set(UNIT_TEST_SOURCES
    put_get.cc
    set_get_attrs.cc
    list_attrs.cc
    copy.cc
    remove.cc
    move.cc
    release.cc
    locate.cc
    list.cc
    get_tiers_info.cc
    list_tiers.cc
    hsm_object_store/motr/TestMockMotrBackend.cc
    hsm_object_store/motr/TestMockMotrHsm.cc
    hsm_object_store/motr/TestMotrHsmClient.cc
)

add_executable(unit_tests ${UNIT_TEST_SOURCES})
target_compile_options(unit_tests PRIVATE -Wunused-parameter)

target_link_libraries(unit_tests PRIVATE 
    hestia_client 
    hestia_motr_plugin_mock
    hestia_lib 
    Catch2::Catch2WithMain)
catch_discover_tests(unit_tests)
endif()
