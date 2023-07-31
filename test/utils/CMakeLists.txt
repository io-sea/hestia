include(AddModule)

configure_file(TestDirectories.h.in ${PROJECT_BINARY_DIR}/TestDirectories.h)

add_module(
    MODULE_NAME test_utils
    HEADERS
        CacheTestFixture.h
        ObjectStoreTestWrapper.h
        HsmObjectStoreTestWrapper.h
        DistributedHsmServiceTestWrapper.h
        TestContext.h
        TestUtils.h
        TestClientConfigs.h
        ProxygenTestUtils.h
    SOURCES
        CacheTestFixture.cc
        ObjectStoreTestWrapper.cc
        HsmObjectStoreTestWrapper.cc
        DistributedHsmServiceTestWrapper.cc
        TestContext.cc
        TestClientConfigs.cc
    INTERNAL_DEPENDENCIES
        web
        hsm_object_store_client
        hsm
        lib
        app
        common
    DEPENDENCIES
        Catch2::Catch2
)

target_include_directories(hestia_test_utils PRIVATE ${PROJECT_BINARY_DIR})

if(${HESTIA_WITH_PROXYGEN})
    target_link_libraries(hestia_test_utils PRIVATE hestia_proxygen_plugin)
    target_link_directories(hestia_test_utils PUBLIC ${zstd_BINARY_DIR})
    target_include_directories(hestia_test_utils PUBLIC ${PROJECT_BINARY_DIR})
    target_compile_definitions(hestia_test_utils PUBLIC HAVE_PROXYGEN)
endif()