add_subdirectory(kv_store)
add_subdirectory(object_store)

add_library(
    hestia_lib

    put.cc
    get.cc
)

target_compile_features(hestia_lib PUBLIC cxx_std_17)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    include(../cmake/development_flags.cmake)
    target_link_libraries(hestia_lib PUBLIC development_flags)
endif()

target_link_libraries(hestia_lib PRIVATE kv_disk obj_disk)

add_executable(
    hestia

    main.cc
)

target_link_libraries(hestia hestia_lib)
