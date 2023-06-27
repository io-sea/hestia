include(AddModule)

add_module(
    MODULE_NAME hsm_base_types
    HEADERS
        HsmObject.h
        StorageTier.h
        TierExtents.h
    SOURCES
        HsmObject.cc
        StorageTier.cc 
        TierExtents.cc
    INTERNAL_DEPENDENCIES 
        common
)