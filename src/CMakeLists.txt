add_subdirectory(../test/kv_store ${CMAKE_CURRENT_BINARY_DIR}/kv_store)
add_subdirectory(../test/object_store ${CMAKE_CURRENT_BINARY_DIR}/object_store)
add_subdirectory(../test/data_placement_engine ${CMAKE_CURRENT_BINARY_DIR}/data_placement_engine)

add_library(
    hestia_lib

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
)

target_compile_features(hestia_lib PUBLIC cxx_std_17)

target_link_libraries(hestia_lib PUBLIC nlohmann_json::nlohmann_json)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(../cmake/development_flags.cmake)
    target_link_libraries(hestia_lib PUBLIC development_flags)
endif()

target_link_libraries(hestia_lib PRIVATE kv_disk obj_disk dpe)

add_executable(
    hestia

    main.cc
)

target_link_libraries(hestia hestia_lib)
