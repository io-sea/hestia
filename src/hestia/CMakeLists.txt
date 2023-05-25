include(AddModule)

add_module(
    MODULE_NAME app
    HEADERS
        HestiaCli.h
        HestiaCppInterface.h
        HestiaService.h
        application/ApplicationContext.h
        application/HestiaConfig.h
        application/HestiaConfigurator.h
        web/http/HestiaWebApp.h 
        web/http/HestiaObjectView.h 
        web/http/HestiaTierView.h 
        web/http/HestiaNodeView.h
        web/s3/HestiaS3WebApp.h
        web/s3/HsmS3Service.h
    SOURCES
        HestiaCli.cc
        HestiaCppInterface.cc
        HestiaService.cc
        application/ApplicationContext.cc
        application/HestiaConfig.cc
        application/HestiaConfigurator.cc
        web/http/HestiaWebApp.cc 
        web/http/HestiaObjectView.cc 
        web/http/HestiaTierView.cc 
        web/http/HestiaNodeView.cc
        web/s3/HestiaS3WebApp.cc
        web/s3/HsmS3Service.cc
    INTERNAL_INCLUDE_DIRS
        application
        web/s3 
        web/http
    INTERNAL_DEPENDENCIES 
        headers
        hsm
        web      
)

target_include_directories(hestia_app SYSTEM PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>)