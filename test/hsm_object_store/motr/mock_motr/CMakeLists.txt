include(ostk/AddModule)

add_module(
    MODULE_NAME motr_plugin_mock 
    HEADERS
        MockMotrInterfaceImpl.h
        MockMotrTypes.h 
        MockMotrBackend.h 
        MockMotr.h 
        MockMotrHsm.h 
        MockMotrHsmInternal.h 
    SOURCES
        MockMotrInterfaceImpl.cc
        MockMotrTypes.cc 
        MockMotrBackend.cc
        MockMotr.cc
        MockMotrHsm.cc
        MockMotrHsmInternal.cc
    INTERNAL_DEPENDENCIES 
        hsm_service
        motr_object_store_client
)

