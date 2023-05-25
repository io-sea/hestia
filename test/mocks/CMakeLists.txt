add_subdirectory(libs3)
add_subdirectory(motr)
add_subdirectory(phobos)

add_module(
    MODULE_NAME mocks 
    HEADERS
        MockS3Server.h
        MockWebService.h
        MockWebView.h
    SOURCES
        MockS3Server.cc
    INTERNAL_DEPENDENCIES 
        protocol
        web
)

add_dependencies(hestia_mocks hestia_mock_phobos_plugin hestia_mock_s3_plugin hestia_mock_motr_plugin)