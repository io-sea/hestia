include(AddModule)

fetch_curl()

add_module(
    MODULE_NAME http_client 
    HEADERS
        HttpClient.h 
        curl/CurlClient.h
        curl/CurlHandle.h
    SOURCES
        curl/CurlClient.cc
        curl/CurlHandle.cc
    INTERNAL_INCLUDE_DIRS
        curl
    INTERNAL_DEPENDENCIES 
        protocol
    DEPENDENCIES 
        CURL::libcurl 
)