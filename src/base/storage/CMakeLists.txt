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
        block_store/Block.h
        block_store/BlockList.h
        block_store/BlockStore.h
        clients/file/FileObjectStoreClient.h
        clients/file/FileKeyValueStoreClient.h
        clients/http/HttpObjectStoreClient.h
        clients/memory/InMemoryObjectStoreClient.h
        clients/memory/InMemoryKeyValueStoreClient.h
        clients/redis/RedisKeyValueStoreClient.h
        requests/ObjectStoreRequest.h
        requests/ObjectStoreResponse.h
        requests/ObjectStoreError.h 
        requests/KeyValueStoreRequest.h
    SOURCES
        ObjectStoreClient.cc
        ObjectStoreClientPlugin.cc
        KeyValueStoreClient.cc
        block_store/Block.cc
        block_store/BlockList.cc
        block_store/BlockStore.cc
        clients/file/FileObjectStoreClient.cc
        clients/file/FileKeyValueStoreClient.cc
        clients/http/HttpObjectStoreClient.cc
        clients/memory/InMemoryObjectStoreClient.cc
        clients/memory/InMemoryKeyValueStoreClient.cc
        clients/redis/RedisKeyValueStoreClient.cc
        requests/ObjectStoreRequest.cc
        requests/ObjectStoreResponse.cc
        requests/ObjectStoreError.cc
        requests/KeyValueStoreRequest.cc
    INTERNAL_INCLUDE_DIRS
        block_store
        clients/file
        clients/http
        clients/memory
        clients/redis
        requests
    INTERNAL_DEPENDENCIES 
        common
        protocol
        http_client
    PRIVATE_DEPENDENCIES
        hiredis::hiredis_static
    WITH_FILESYSTEM
)

add_subdirectory(clients/phobos)
add_subdirectory(clients/s3)