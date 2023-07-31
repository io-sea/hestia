include(FetchProxygen)

find_package(proxygen QUIET)
if (${proxygen_FOUND})
    find_package(gflags REQUIRED) # needed for folly-linker
else()
    fetch_proxygen()
endif()

add_module(
    MODULE_NAME proxygen_plugin
    HEADERS
        ProxygenServer.h
        ProxygenRequestHandler.h
        ProxygenGetRequestHandler.h
        ProxygenPutRequestHandler.h
        ProxygenUnsupportedRequestHandler.h
        ProxygenHandlerFactory.h
        ProxygenMessage.h
    SOURCES
        ProxygenServer.cc
        ProxygenRequestHandler.cc
        ProxygenGetRequestHandler.cc
        ProxygenPutRequestHandler.cc
        ProxygenUnsupportedRequestHandler.cc
        ProxygenHandlerFactory.cc
        ProxygenMessage.cc
    INTERNAL_DEPENDENCIES
        server
    DEPENDENCIES
        proxygenhttpserver
    COMPILER_DEFINITIONS
        HAVE_PROXYGEN
)

target_compile_options(hestia_proxygen_plugin PUBLIC -Wno-deprecated-declarations -Wno-nullability-completeness)
add_dependencies(hestia_server hestia_proxygen_plugin)