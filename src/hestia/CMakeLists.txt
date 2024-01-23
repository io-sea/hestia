include(AddModule)

file(READ web/http/index.html HESTIA_LANDING_PAGE)
configure_file(web/http/HestiaWebPages.h.in ${PROJECT_BINARY_DIR}/HestiaWebPages.h)

add_module(
    MODULE_NAME app
    HEADERS
        HestiaCli.h
        HestiaClient.h
        HestiaServer.h
        application/HestiaConfig.h
        application/HestiaCache.h
        application/HestiaApplication.h
        application/HestiaCommands.h
        application/HestiaRequest.h 
        application/HestiaResponse.h
        application/ServerConfig.h
        web/http/HestiaWebApp.h 
        web/http/HestiaHsmActionView.h 
        web/http/HestiaUserAuthView.h
        web/s3/HestiaS3WebApp.h
        web/s3/S3UrlRouter.h
        web/s3/middleware/S3AuthenticationMiddleware.h 
        web/s3/middleware/S3AuthorisationChecker.h
        web/s3/views/S3BucketView.h 
        web/s3/views/S3ObjectView.h
        web/s3/views/S3WebView.h 
        web/s3/views/S3BucketListView.h
    SOURCES
        HestiaCApi.cc
        HestiaCli.cc
        HestiaClient.cc
        HestiaServer.cc
        HestiaTypes.cc
        application/HestiaConfig.cc
        application/HestiaCache.cc
        application/HestiaApplication.cc
        application/HestiaCommands.cc
        application/ServerConfig.cc
        application/HestiaRequest.cc 
        application/HestiaResponse.cc
        web/http/HestiaWebApp.cc 
        web/http/HestiaHsmActionView.cc 
        web/http/HestiaUserAuthView.cc
        web/s3/HestiaS3WebApp.cc
        web/s3/S3UrlRouter.cc
        web/s3/middleware/S3AuthenticationMiddleware.cc 
        web/s3/middleware/S3AuthorisationChecker.cc
        web/s3/views/S3BucketView.cc 
        web/s3/views/S3ObjectView.cc
        web/s3/views/S3WebView.cc
        web/s3/views/S3BucketListView.cc
        web/s3/views/S3ViewUtils.cc
    INTERNAL_INCLUDE_DIRS
        application
        web/s3 
        web/s3/middleware
        web/s3/views
        web/http
    INTERNAL_DEPENDENCIES 
        headers
        hsm
        server     
        cli 
        http_client
)

target_include_directories(hestia_app SYSTEM PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external>)