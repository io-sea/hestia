include(AddModule)

add_module(
    MODULE_NAME client
    HEADERS
        hestia_old.h
        HestiaConfig.h
        HestiaConfigurator.h
        HestiaCli.h
        HestiaCppInterface.h
        HestiaService.h
        application/ApplicationContext.h
        application/ConfigParser.h
        web/http/HestiaWebApp.h 
        web/http/HestiaWebView.h 
        web/s3/HestiaS3WebApp.h
        web/s3/HsmS3Service.h
    SOURCES
        hestia.cc
        HestiaConfig.cc
        HestiaConfigurator.cc
        HestiaCli.cc
        HestiaCppInterface.cc
        HestiaService.cc
        application/ApplicationContext.cc
        application/ConfigParser.cc
        web/http/HestiaWebApp.cc 
        web/http/HestiaWebView.cc
        web/s3/HestiaS3WebApp.cc
        web/s3/HsmS3Service.cc
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