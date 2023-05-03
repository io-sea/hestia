set(MODULE_NAME hestia_mock_s3_server)

set(SOURCES
    MockS3Server.h
    MockS3Server.cc
)

add_library(${MODULE_NAME} ${SOURCES})

target_include_directories(${MODULE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${MODULE_NAME} PRIVATE hestia_protocol)
