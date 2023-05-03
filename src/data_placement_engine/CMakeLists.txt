include(AddModule)

add_module(
    MODULE_NAME data_placement_engine 
    HEADERS
        HsmTier.h
        DataPlacementEngine.h
        DataPlacementEngineRegistry.h
        BasicDataPlacementEngine.h
    SOURCES
        BasicDataPlacementEngine.cc
    DEPENDENCIES
        hestia::hestia
)