include(AddModule)

add_module(
    MODULE_NAME hsm_base_types
    HEADERS
        HsmObject.h
        HsmObjectAdapter.h
        StorageTier.h
        StorageTierAdapter.h
    SOURCES
        HsmObject.cc
        HsmObjectAdapter.cc
        StorageTierAdapter.cc
    INTERNAL_DEPENDENCIES 
        common
)