include(AddModule)

add_module(
    MODULE_NAME app
    HEADERS
        HestiaCli.h
        HestiaCppInterface.h
        application/ApplicationContext.h
        application/HestiaConfig.h
        application/HestiaConfigurator.h
        hestia_service/HestiaService.h
        hestia_service/HestiaNodeService.h
        hestia_service/HestiaNode.h
        hestia_service/HestiaNodeAdapter.h
        web/http/HestiaWebApp.h 
        web/http/HestiaObjectView.h 
        web/http/HestiaTierView.h 
        web/http/HestiaNodeView.h
        web/s3/HestiaS3WebApp.h
        web/s3/HsmS3Service.h
    SOURCES
        HestiaCli.cc
        HestiaCppInterface.cc
        application/ApplicationContext.cc
        application/HestiaConfig.cc
        application/HestiaConfigurator.cc
        hestia_service/HestiaService.cc
        hestia_service/HestiaNodeService.cc
        hestia_service/HestiaNodeAdapter.cc
        web/http/HestiaWebApp.cc 
        web/http/HestiaObjectView.cc 
        web/http/HestiaTierView.cc 
        web/http/HestiaNodeView.cc
        web/s3/HestiaS3WebApp.cc
        web/s3/HsmS3Service.cc
    INTERNAL_INCLUDE_DIRS
        application
        hestia_service
        web/s3 
        web/http
    INTERNAL_DEPENDENCIES 
        headers
        hsm
        web      
)

target_include_directories(hestia_app SYSTEM PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>)