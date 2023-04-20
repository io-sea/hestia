include(AddModule)

add_module(
    MODULE_NAME copytool_lib 
    HEADERS
        CopyTool.h 
        CopyToolRequest.h 
        CopyToolResponse.h 
        CopyToolError.h 
        CopyToolConnection.h 
    SOURCES
        CopyTool.cc 
        CopyToolRequest.cc
        CopyToolResponse.cc
        CopyToolConnection.cc
    INTERNAL_DEPENDENCIES 
        hsm_object_store_client
)

add_executable(hestia_copytool main.cc)
target_link_libraries(hestia_copytool PUBLIC hestia_copytool_lib)