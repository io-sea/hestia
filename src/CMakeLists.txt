include(AddModule)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(development_flags)
endif()

configure_file(ProjectConfig.h.in ${PROJECT_BINARY_DIR}/ProjectConfig.h)

add_library(hestia_headers INTERFACE)
target_include_directories(hestia_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_subdirectory(base)
add_subdirectory(hsm)
add_subdirectory(hestia)

add_executable(hestia_main main.cc)
target_link_libraries(hestia_main PRIVATE hestia_app)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_link_libraries(hestia_main PRIVATE development_flags)
endif()
if(NOT APPLE)
    target_link_libraries(hestia_main PRIVATE stdc++fs)
endif()

set_target_properties(${PROJECT_NAME}_main PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

add_module(
    MODULE_NAME lib
    SHARED
    WITH_FILESYSTEM
    HEADERS
        hestia.h
    SOURCES
        hestia/hestia.cc
    PRIVATE_DEPENDENCIES
        ${PROJECT_NAME}_app
)

target_sources(${PROJECT_NAME}_lib PUBLIC FILE_SET 
    HEADERS BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/hestia.h)

set_target_properties(${PROJECT_NAME}_lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})