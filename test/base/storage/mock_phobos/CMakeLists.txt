include(AddModule)

add_module(
    MODULE_NAME mock_phobos 
    HEADERS
        MockPhobos.h
        MockPhobosInterface.h
        MockPhobosClient.h
    SOURCES
        MockPhobos.cc
        MockPhobosInterface.cc
        MockPhobosClient.cc
    INTERNAL_DEPENDENCIES 
        storage
        phobos_backend_interface_impl
        phobos_backend
)

add_library(${PROJECT_NAME}_mock_phobos_plugin SHARED 
    MockPhobosClientPlugin.cc
)

target_link_libraries(${PROJECT_NAME}_mock_phobos_plugin PUBLIC 
    hestia_mock_phobos)
