include(AddModule)

add_module(
    MODULE_NAME client
    HEADERS
        hestia.h
        HestiaConfig.h
        HestiaCppInterface.h
        application/ApplicationContext.h
        application/ConfigParser.h
    SOURCES
        hestia.cc
        HestiaConfig.cc
        HestiaCppInterface.cc
        application/ApplicationContext.cc
        application/ConfigParser.cc
    INTERNAL_INCLUDE_DIRS
        application
    INTERNAL_DEPENDENCIES 
        hsm_service    
)