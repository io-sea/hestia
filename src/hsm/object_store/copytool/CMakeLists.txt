include(AddModule)

add_module(
    MODULE_NAME copytool_lib 
    HEADERS
        CopyTool.h 
        CopyToolRequest.h 
        CopyToolResponse.h 
        CopyToolError.h 
        CopyToolConnection.h 
        CopyToolWebApp.h 
        CopyToolWebClient.h
    SOURCES
        CopyTool.cc 
        CopyToolRequest.cc
        CopyToolResponse.cc
        CopyToolConnection.cc
        CopyToolWebApp.cc 
        CopyToolWebClient.cc
    INTERNAL_DEPENDENCIES 
        hsm_object_store_client
        web
        http_client
)

add_executable(hestia_copytool main.cc)
target_link_libraries(hestia_copytool PUBLIC hestia_copytool_lib)