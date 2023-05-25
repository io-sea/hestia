include(AddModule)

add_module(
    MODULE_NAME web 
    HEADERS
        WebApp.h 
        WebView.h 
        ApplicationMiddleware.h 
        UrlRouter.h 
        authentication/AuthenticationMiddleware.h 
        authentication/AuthenticationSession.h 
        authentication/User.h 
        authentication/UserService.h
    SOURCES
        WebApp.cc
        WebView.cc
        UrlRouter.cc
        authentication/UserService.cc
    INTERNAL_DEPENDENCIES 
        storage 
        protocol
    INTERNAL_INCLUDE_DIRS 
        authentication
)

add_subdirectory(s3)