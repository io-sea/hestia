include(AddModule)

configure_file(TestDirectories.h.in ${PROJECT_BINARY_DIR}/TestDirectories.h)

add_module(
    MODULE_NAME test_utils 
    HEADERS
        MockWebService.h
        MockWebView.h
        TestContext.h
        TestUtils.h
    SOURCES
        TestContext.cc
    INTERNAL_DEPENDENCIES 
        web
)

target_include_directories(hestia_test_utils PRIVATE ${PROJECT_BINARY_DIR})