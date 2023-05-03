include(AddModule)

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(copytool)

add_module(
    MODULE_NAME object_store 
    HEADERS
        CopyToolInterface.h
        MultiBackendHsmObjectStoreClient.h      
    SOURCES
        CopyToolInterface.cc
        MultiBackendHsmObjectStoreClient.cc
    INTERNAL_DEPENDENCIES 
        copytool_lib
        hsm_object_store_client
)