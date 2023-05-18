include(AddModule)

configure_file(TestDirectories.h.in ${PROJECT_BINARY_DIR}/TestDirectories.h)

add_module(
    MODULE_NAME test_utils 
    HEADERS
        MockWebService.h
        MockWebView.h
        TestContext.h
        TestUtils.h
      #  HsmTestContext.h
    SOURCES
        TestContext.cc
       # HsmTestContext.cc
    INTERNAL_DEPENDENCIES 
        web
        hsm_object_store_client
)

target_include_directories(hestia_test_utils PRIVATE ${PROJECT_BINARY_DIR})