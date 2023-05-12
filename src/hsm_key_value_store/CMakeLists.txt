include(AddModule)

add_module(
    MODULE_NAME hsm_key_value_store 
    WITH_FILESYSTEM
    HEADERS
        KeyValueStoreClientRegistry.h
        HsmKeyValueStore.h 
    SOURCES
        HsmKeyValueStore.cc
        KeyValueStoreClientRegistry.cc
    INTERNAL_DEPENDENCIES
        storage
)