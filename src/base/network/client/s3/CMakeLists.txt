include(AddModule)

add_module(
    MODULE_NAME s3_client 
    HEADERS
        S3Client.h 
        S3Responses.h
    SOURCES
        S3Client.cc 
        S3Responses.cc
    INTERNAL_DEPENDENCIES 
        http_client
)