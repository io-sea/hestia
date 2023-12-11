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
    SOURCES
        CacheTestFixture.cc
        ObjectStoreTestWrapper.cc
        HsmObjectStoreTestWrapper.cc
        DistributedHsmServiceTestWrapper.cc
        TestContext.cc
        TestClientConfigs.cc
    INTERNAL_DEPENDENCIES
        web
        hsm
        lib
        app
        common
    DEPENDENCIES
        Catch2::Catch2
)

target_include_directories(hestia_test_utils PRIVATE ${PROJECT_BINARY_DIR})