add_subdirectory(kv_store)
add_subdirectory(object_store)

add_library(
    hestia_lib

    put.cc
)

target_compile_features(hestia_lib PUBLIC cxx_std_17)

target_link_libraries(hestia_lib kv_disk obj_disk)

add_executable(
    hestia

    main.cc
)

target_link_libraries(hestia hestia_lib)
