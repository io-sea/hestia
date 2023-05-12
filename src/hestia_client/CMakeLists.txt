include(AddModule)

add_module(
    MODULE_NAME client
    HEADERS
        hestia_old.h
        HestiaConfig.h
        HestiaConfigurator.h
        HestiaCli.h
        HestiaCppInterface.h
        application/ApplicationContext.h
        application/ConfigParser.h
    SOURCES
        hestia.cc
        HestiaConfig.cc
        HestiaConfigurator.cc
        HestiaCli.cc
        HestiaCppInterface.cc
        application/ApplicationContext.cc
        application/ConfigParser.cc
    INTERNAL_INCLUDE_DIRS
        application
    INTERNAL_DEPENDENCIES 
        hsm_service
    DEPENDENCIES  
        nlohmann_json::nlohmann_json       
)

target_include_directories(hestia_client SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/external)