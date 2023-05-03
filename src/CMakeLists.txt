add_subdirectory(base)
add_subdirectory(data_placement_engine)
add_subdirectory(event_feed)
add_subdirectory(key_value_store)
add_subdirectory(hsm_object_store)
add_subdirectory(hsm_service)
add_subdirectory(hestia_client)

include(FetchDependencies)
fetch_nlohmann_json()

add_library(dpe data_placement_engine/eejit.cc)
target_link_libraries(dpe PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(dpe PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(kv_disk key_value_store/disk.cc)
target_link_libraries(kv_disk PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(kv_disk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(obj_disk hsm_object_store/disk.cc)
target_link_libraries(obj_disk PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(obj_disk PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

add_library(hestia_lib SHARED 
    put.cc
    get.cc
    set_attrs.cc
    get_attrs.cc
    list_attrs.cc
    remove.cc
    list.cc
    copy.cc
    move.cc
    release.cc
    get_tiers_info.cc
    list_tiers.cc
    locate.cc
)

target_link_libraries(hestia_lib PUBLIC dpe kv_disk obj_disk)
if(NOT APPLE)
    target_link_libraries(hestia_lib PUBLIC stdc++fs)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(development_flags)
    target_link_libraries(hestia_lib PUBLIC development_flags)
endif()

add_executable(hestia_main main.cc)

target_link_libraries(hestia_main hestia_lib)

set(HESTIA_PUBLIC_HEADERS
    hestia.h
    types.h
    )
set_target_properties(hestia_lib PROPERTIES PUBLIC_HEADER "${HESTIA_PUBLIC_HEADERS}")
install(TARGETS hestia_lib
        LIBRARY DESTINATION lib64
        PUBLIC_HEADER DESTINATION include)
