include(AddModule)

set(OPTIONAL_HEADERS)
set(OPTIONAL_SOURCES)
set(OPTIONAL_DEPENDENCIES)
set(OPTIONAL_INCLUDE_DIRS)

fetch_curl()
list(APPEND OPTIONAL_HEADERS 
    curl/CurlClient.h
    curl/CurlHandle.h
    )
list(APPEND OPTIONAL_SOURCES 
    curl/CurlClient.cc
    curl/CurlHandle.cc
    )
list(APPEND OPTIONAL_DEPENDENCIES CURL::libcurl)
list(APPEND OPTIONAL_INCLUDE_DIRS curl)

add_module(
    MODULE_NAME http_client 
    HEADERS
        HttpClient.h 
        ${OPTIONAL_HEADERS}
    SOURCES
        HttpClient.cc 
        ${OPTIONAL_SOURCES}
    INTERNAL_DEPENDENCIES 
        protocol
    DEPENDENCIES 
        ${OPTIONAL_DEPENDENCIES} 
    INTERNAL_INCLUDE_DIRS
        ${OPTIONAL_INCLUDE_DIRS} 
)