include(ostk/AddModule)

add_module(
    MODULE_NAME motr_object_store_client 
    HEADERS
        MotrHsmClient.h
        MotrInterface.h 
        IMotrInterfaceImpl.h
    SOURCES
        MotrHsmClient.cc
        MotrInterface.cc 
    INTERNAL_DEPENDENCIES 
        hsm_object_store_client
)