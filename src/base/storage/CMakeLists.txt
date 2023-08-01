include(AddModule)

find_package(hiredis QUIET)
if(NOT ${hiredis_FOUND})
    fetch_hiredis()
endif()

add_module(
    MODULE_NAME storage 
    HEADERS
        Database.h 
        ObjectStoreClient.h 
        ObjectStoreClientPlugin.h 
        KeyValueStoreClient.h
        base_types/StorageObject.h
        base_types/Extent.h
        block_store/Block.h
        block_store/BlockList.h
        block_store/BlockStore.h
        clients/file/FileObjectStoreClient.h
        clients/file/FileHsmObjectStoreClient.h
        clients/file/FileKeyValueStoreClient.h
        clients/memory/InMemoryObjectStoreClient.h
        clients/memory/InMemoryHsmObjectStoreClient.h
        clients/memory/InMemoryKeyValueStoreClient.h
        clients/redis/RedisKeyValueStoreClient.h
        hsm/HsmObjectStoreClient.h
        hsm/HsmObjectStoreClientPlugin.h
        requests/HsmObjectStoreError.h
        requests/HsmObjectStoreRequest.h
        requests/HsmObjectStoreResponse.h
        requests/ObjectStoreRequest.h
        requests/ObjectStoreResponse.h
        requests/ObjectStoreError.h 
        requests/KeyValueStoreRequest.h
        requests/KeyValueStoreResponse.h
    SOURCES
        ObjectStoreClient.cc
        ObjectStoreClientPlugin.cc
        KeyValueStoreClient.cc
        base_types/StorageObject.cc
        base_types/Extent.cc
        block_store/Block.cc
        block_store/BlockList.cc
        block_store/BlockStore.cc
        clients/file/FileObjectStoreClient.cc
        clients/file/FileHsmObjectStoreClient.cc
        clients/file/FileKeyValueStoreClient.cc
        clients/memory/InMemoryObjectStoreClient.cc
        clients/memory/InMemoryHsmObjectStoreClient.cc
        clients/memory/InMemoryKeyValueStoreClient.cc
        clients/redis/RedisKeyValueStoreClient.cc
        hsm/HsmObjectStoreClient.cc
        hsm/HsmObjectStoreClientPlugin.cc
        requests/HsmObjectStoreError.cc
        requests/HsmObjectStoreRequest.cc
        requests/HsmObjectStoreResponse.cc
        requests/ObjectStoreRequest.cc
        requests/ObjectStoreResponse.cc
        requests/ObjectStoreError.cc
        requests/KeyValueStoreRequest.cc
        requests/KeyValueStoreResponse.cc
    INTERNAL_INCLUDE_DIRS
        base_types
        block_store
        clients/file
        clients/memory
        clients/redis
        hsm
        requests
    INTERNAL_DEPENDENCIES 
        common
        protocol
        http_client
    PRIVATE_DEPENDENCIES
        hiredis::hiredis
    WITH_FILESYSTEM
)

add_custom_target(${PROJECT_NAME}_storage_and_plugins)
add_dependencies(${PROJECT_NAME}_storage_and_plugins ${PROJECT_NAME}_storage)

add_subdirectory(clients/phobos)
add_subdirectory(clients/motr)
add_subdirectory(clients/s3)