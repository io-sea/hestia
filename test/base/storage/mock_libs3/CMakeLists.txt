include(AddModule)

add_module(
    MODULE_NAME mock_s3 
    HEADERS
        MockLibS3InterfaceImpl.h
        MockLibS3.h
    SOURCES
        MockLibS3InterfaceImpl.cc
        MockLibS3.cc
    INTERNAL_DEPENDENCIES 
        storage
        s3_backend
        mock_s3_server
)

add_library(${PROJECT_NAME}_mock_s3_plugin SHARED 
    MockS3ClientPlugin.cc
)

target_link_libraries(${PROJECT_NAME}_mock_s3_plugin PUBLIC 
    hestia_mock_s3)