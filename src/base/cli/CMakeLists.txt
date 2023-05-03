include(AddModule)

add_module(
    MODULE_NAME cli 
    HEADERS
        DaemonManager.h
        CommandLineArgs.h
    SOURCES
        DaemonManager.cc
        CommandLineArgs.cc
    INTERNAL_DEPENDENCIES 
        common
    WITH_FILESYSTEM
)
