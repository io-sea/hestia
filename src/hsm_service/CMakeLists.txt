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
    include(development_flags)
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