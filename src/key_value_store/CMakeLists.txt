include(AddModule)

add_module(
    MODULE_NAME key_value_store 
    WITH_FILESYSTEM
    HEADERS
        client/file/FileKeyValueStoreClient.h
        KeyValueStoreClient.h
        KeyValueStoreClientRegistry.h
        KeyValueStore.h 
    SOURCES
        client/file/FileKeyValueStoreClient.cc
        KeyValueStore.cc
        KeyValueStoreClient.cc
    INTERNAL_INCLUDE_DIRS
        client/file/
    INTERNAL_DEPENDENCIES
        storage
)