include(AddModule)

add_module(
    MODULE_NAME cli 
    HEADERS
        DaemonManager.h
        ConsoleInterface.h
    SOURCES
        DaemonManager.cc
        ConsoleInterface.cc
    INTERNAL_DEPENDENCIES 
        common
    WITH_FILESYSTEM
)
