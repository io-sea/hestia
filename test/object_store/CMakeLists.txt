add_library(obj_disk
    disk.cc
)

target_compile_features(obj_disk PUBLIC cxx_std_17)
target_link_libraries(obj_disk PRIVATE nlohmann_json::nlohmann_json)
