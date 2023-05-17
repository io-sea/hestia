if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(development_flags)
endif()

add_library(hestia_headers INTERFACE)
target_include_directories(hestia_headers INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_subdirectory(base)
add_subdirectory(data_placement_engine)
add_subdirectory(event_feed)
add_subdirectory(hsm_key_value_store)
add_subdirectory(hsm_object_store)
add_subdirectory(hsm_service)
add_subdirectory(hestia_client)

add_executable(hestia_main main.cc)
target_link_libraries(hestia_main hestia_client)
set_target_properties(hestia_main PROPERTIES OUTPUT_NAME "hestia")