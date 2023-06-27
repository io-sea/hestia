include(AddModule)

add_module(
    MODULE_NAME web 
    HEADERS
        WebApp.h 
        WebView.h 
        ApplicationMiddleware.h 
        StaticContentView.h 
        UrlRouter.h 
        authentication/AuthenticationMiddleware.h 
        authentication/TokenAuthenticationMiddleware.h
        authentication/AuthenticationSession.h 
        authentication/UserService.h
    SOURCES
        StaticContentView.cc 
        WebApp.cc
        WebView.cc
        UrlRouter.cc
        authentication/UserService.cc
        authentication/TokenAuthenticationMiddleware.cc
    INTERNAL_DEPENDENCIES 
        storage 
        protocol
    INTERNAL_INCLUDE_DIRS 
        authentication
)