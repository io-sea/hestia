include(AddModule)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(development_flags)
endif()

configure_file(ProjectConfig.h.in ${PROJECT_BINARY_DIR}/ProjectConfig.h)
install(FILES ${PROJECT_BINARY_DIR}/ProjectConfig.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

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

set_target_properties(hestia_main PROPERTIES OUTPUT_NAME "hestia")

add_module(
    MODULE_NAME lib
    SHARED
    WITH_FILESYSTEM
    HEADERS
        hestia.h
    SOURCES
        hestia/hestia.cc
    INTERNAL_DEPENDENCIES 
        headers
        app  
)