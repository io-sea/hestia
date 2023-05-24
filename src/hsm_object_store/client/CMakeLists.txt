include(AddModule)

add_module(
    MODULE_NAME hsm_object_store_client 
    HEADERS
        HsmObjectStoreClient.h
        HsmObjectStoreClientSpec.h
        HsmObjectStoreClientPlugin.h
        HsmObjectStoreClientManager.h
        HsmObjectStoreClientFactory.h
        file/FileHsmObjectStoreClient.h
    SOURCES
        HsmObjectStoreClient.cc
        HsmObjectStoreClientSpec.cc
        HsmObjectStoreClientPlugin.cc
        HsmObjectStoreClientManager.cc
        HsmObjectStoreClientFactory.cc
        file/FileHsmObjectStoreClient.cc
    INTERNAL_INCLUDE_DIRS
        file
    INTERNAL_DEPENDENCIES 
        hsm_object_store_common
)

add_subdirectory(motr)