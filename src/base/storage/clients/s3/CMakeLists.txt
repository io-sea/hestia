include(AddModule)
include(FetchDependencies)

add_module(
    MODULE_NAME s3_backend
    HEADERS
        S3Client.h
        IS3InterfaceImpl.h
    SOURCES
        S3Client.cc
    INTERNAL_DEPENDENCIES 
        storage
)

if(HESTIA_WITH_S3_CLIENT)
    fetch_curl()
    fetch_libxml2()
    fetch_libs3()

    add_library(${PROJECT_NAME}_s3_plugin SHARED
        LibS3InterfaceImpl.h 
        LibS3InterfaceImpl.cc
        S3ClientPlugin.cc
    )

    target_link_libraries(${PROJECT_NAME}_s3_plugin PRIVATE 
        hestia_s3_backend
        libs3::s3)

    add_dependencies(${PROJECT_NAME}_storage_and_plugins ${PROJECT_NAME}_s3_plugin)
endif()