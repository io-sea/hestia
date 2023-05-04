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

add_executable(hestia_main main.cc)
target_link_libraries(hestia_main hestia_lib)
