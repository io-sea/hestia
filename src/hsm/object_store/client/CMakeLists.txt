include(AddModule)

add_module(
    MODULE_NAME hsm_object_store_client 
    HEADERS
        HsmObjectStoreClient.h
        HsmObjectStoreClientBackend.h
        HsmObjectStoreClientPlugin.h
        HsmObjectStoreClientManager.h
        HsmObjectStoreClientFactory.h
        file/FileHsmObjectStoreClient.h
        memory/InMemoryHsmObjectStoreClient.h
    SOURCES
        HsmObjectStoreClient.cc
        HsmObjectStoreClientBackend.cc
        HsmObjectStoreClientPlugin.cc
        HsmObjectStoreClientManager.cc
        HsmObjectStoreClientFactory.cc
        file/FileHsmObjectStoreClient.cc
        memory/InMemoryHsmObjectStoreClient.cc
    INTERNAL_INCLUDE_DIRS
        file
        memory
    INTERNAL_DEPENDENCIES 
        hsm_object_store_common
        hsm_base_types
)

add_subdirectory(motr)