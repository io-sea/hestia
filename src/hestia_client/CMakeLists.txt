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
        web/http/HestiaWebApp.h 
        web/s3/HestiaS3WebApp.h
    SOURCES
        hestia.cc
        HestiaConfig.cc
        HestiaConfigurator.cc
        HestiaCli.cc
        HestiaCppInterface.cc
        application/ApplicationContext.cc
        application/ConfigParser.cc
        web/http/HestiaWebApp.cc 
        web/s3/HestiaS3WebApp.cc
    INTERNAL_INCLUDE_DIRS
        application
        web/s3 
        web/http
    INTERNAL_DEPENDENCIES 
        hsm_service
        web
    DEPENDENCIES  
        nlohmann_json::nlohmann_json       
)

target_include_directories(hestia_client SYSTEM PUBLIC ${PROJECT_SOURCE_DIR}/external)