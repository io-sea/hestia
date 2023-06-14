include(AddModule)

add_subdirectory(common)
add_subdirectory(client)

add_module(
    MODULE_NAME distributed_hsm_object_store 
    HEADERS
        DistributedHsmObjectStoreClient.h      
    SOURCES
        DistributedHsmObjectStoreClient.cc
    INTERNAL_DEPENDENCIES 
        hsm
)