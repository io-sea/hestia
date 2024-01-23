include(AddModule)

add_module(
    MODULE_NAME cli 
    HEADERS
        ConsoleInterface.h
    SOURCES
        ConsoleInterface.cc
    INTERNAL_DEPENDENCIES 
        common
    WITH_FILESYSTEM
)
