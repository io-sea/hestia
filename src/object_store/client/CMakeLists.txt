include(AddModule)

add_module(
    MODULE_NAME hsm_object_store_client 
    HEADERS
        HsmObjectStoreClient.h
        HsmObjectStoreClientPlugin.h
        HsmObjectStoreClientManager.h
        HsmObjectStoreClientRegistry.h
        file/FileHsmObjectStoreClient.h
    SOURCES
        HsmObjectStoreClient.cc
        HsmObjectStoreClientPlugin.cc
        HsmObjectStoreClientManager.cc
        HsmObjectStoreClientRegistry.cc
        file/FileHsmObjectStoreClient.cc
    INTERNAL_INCLUDE_DIRS
        file
    INTERNAL_DEPENDENCIES 
        hsm_object_store_common
)

add_subdirectory(motr)