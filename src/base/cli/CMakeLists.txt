include(AddModule)

add_module(
    MODULE_NAME cli 
    HEADERS
        DaemonManager.h
    SOURCES
        DaemonManager.cc
    INTERNAL_DEPENDENCIES 
        common
    WITH_FILESYSTEM
)
