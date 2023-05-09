cmake_minimum_required(VERSION 3.0)

project(libs3 VERSION 4.1 LANGUAGES C CXX)

find_package(CURL QUIET)
find_package(LibXml2 QUIET)

add_library(s3 STATIC
    src/bucket.c 
    src/bucket_metadata.c 
    src/error_parser.c 
    src/general.c
    src/object.c 
    src/request.c 
    src/request_context.c
    src/response_headers_handler.c 
    src/service_access_logging.c
    src/service.c 
    src/simplexml.c 
    src/util.c 
    src/multipart.c
)

target_compile_definitions(s3 PUBLIC LIBS3_VER_MAJOR="4" LIBS3_VER_MINOR="1")
target_include_directories(s3 PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>")
target_link_libraries(s3 PUBLIC CURL::libcurl LibXml2::LibXml2)

include(GNUInstallDirs)
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(FILES inc/libs3.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS s3
        EXPORT ${PROJECT_NAME}-targets
        LIBRARY DESTINATION lib/${PROJECT_NAME}
        ARCHIVE DESTINATION lib/${PROJECT_NAME}
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

install(EXPORT ${PROJECT_NAME}-targets
        FILE LibS3Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION lib/cmake/${PROJECT_NAME}
         )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    LibS3ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(LibS3Config.cmake.in LibS3Config.cmake @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/LibS3Config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/LibS3ConfigVersion.cmake"
        DESTINATION lib/cmake/${PROJECT_NAME}
        )


