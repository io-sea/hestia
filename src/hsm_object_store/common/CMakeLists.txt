include(ostk/AddModule)

add_module(
    MODULE_NAME hsm_object_store_common 
    HEADERS
        HsmObjectStoreRequest.h
        HsmObjectStoreResponse.h
        HsmMiddlewareResponse.h
        HsmObjectStoreError.h
    SOURCES
        HsmObjectStoreRequest.cc
        HsmObjectStoreResponse.cc
        HsmMiddlewareResponse.cc
        HsmObjectStoreError.cc
    DEPENDENCIES
        ostk::ostk
)