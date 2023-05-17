include(AddModule)

add_module(
    MODULE_NAME motr_plugin_mock 
    HEADERS
        MockMotrInterfaceImpl.h
        MockMotrTypes.h 
        MockMotrBackend.h 
        MockMotr.h 
        MockMotrHsm.h 
        MockMotrHsmInternal.h 
        MockMotrClient.h
    SOURCES
        MockMotrInterfaceImpl.cc
        MockMotrTypes.cc 
        MockMotrBackend.cc
        MockMotr.cc
        MockMotrHsm.cc
        MockMotrHsmInternal.cc
        MockMotrClient.cc
    INTERNAL_DEPENDENCIES 
        hsm_service
        motr_object_store_client

)

add_library(${PROJECT_NAME}_mock_motr_plugin SHARED 
    MockMotrClientPlugin.cc
)

target_link_libraries(${PROJECT_NAME}_mock_motr_plugin PUBLIC 
    hestia_motr_plugin_mock)

