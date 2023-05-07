include(AddModule)

add_module(
    MODULE_NAME hsm_service 
    HEADERS
        HsmService.h
        HsmStoreInterface.h
        model/HsmAction.h
        model/HsmActionAdapter.h
        model/HsmObjectAdapter.h
        model/HsmObject.h
        model/CompositeLayer.h
        requests/HsmServiceResponse.h 
        requests/HsmServiceRequest.h
        requests/HsmServiceError.h 
    SOURCES
        HsmService.cc
        HsmStoreInterface.cc
        model/HsmAction.cc
        model/HsmActionAdapter.cc
        model/HsmObjectAdapter.cc
        model/HsmObject.cc
        model/CompositeLayer.cc
        requests/HsmServiceResponse.cc
        requests/HsmServiceRequest.cc
        requests/HsmServiceError.cc
    INTERNAL_INCLUDE_DIRS
        model
        requests
    INTERNAL_DEPENDENCIES 
        object_store
        key_value_store 
        data_placement_engine
        event_feed
)

include(FetchDependencies)
fetch_nlohmann_json()

add_library(dpe ${PROJECT_SOURCE_DIR}/src/data_placement_engine/eejit.cc)
target_link_libraries(dpe PUBLIC nlohmann_json::nlohmann_json)

if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(dpe PUBLIC development_flags)
endif()

target_include_directories(dpe PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

add_library(kv_disk ${PROJECT_SOURCE_DIR}/src/key_value_store/disk.cc)
target_link_libraries(kv_disk PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(kv_disk PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(kv_disk PUBLIC development_flags)
endif()

add_library(obj_disk ${PROJECT_SOURCE_DIR}/src/hsm_object_store/disk.cc)
target_link_libraries(obj_disk PUBLIC nlohmann_json::nlohmann_json)
target_include_directories(obj_disk PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>)

if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(obj_disk PUBLIC development_flags)
endif()

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
target_include_directories(hestia_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
if(NOT APPLE)
    target_link_libraries(hestia_lib PUBLIC stdc++fs)
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug)
target_link_libraries(hestia_lib PUBLIC development_flags)
endif()


set(HESTIA_PUBLIC_HEADERS
    hestia.h
    types.h
    )
set_target_properties(hestia_lib PROPERTIES PUBLIC_HEADER "${HESTIA_PUBLIC_HEADERS}")
install(TARGETS hestia_lib
        LIBRARY DESTINATION lib64
        PUBLIC_HEADER DESTINATION include)