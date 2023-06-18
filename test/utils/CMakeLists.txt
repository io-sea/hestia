include(AddModule)

configure_file(TestDirectories.h.in ${PROJECT_BINARY_DIR}/TestDirectories.h)

add_module(
    MODULE_NAME test_utils 
    HEADERS
        ObjectStoreTestWrapper.h
        HsmObjectStoreTestWrapper.h
        DistributedHsmServiceTestWrapper.h
        TestContext.h
        TestUtils.h
    SOURCES
        ObjectStoreTestWrapper.cc
        HsmObjectStoreTestWrapper.cc
        DistributedHsmServiceTestWrapper.cc
        TestContext.cc
    INTERNAL_DEPENDENCIES 
        web
        hsm_object_store_client
        hsm
    DEPENDENCIES 
        Catch2::Catch2
)

target_include_directories(hestia_test_utils PRIVATE ${PROJECT_BINARY_DIR})