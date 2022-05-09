add_library(kv_disk
    disk.cc
)

target_compile_features(kv_disk PUBLIC cxx_std_17)
target_link_libraries(kv_disk PRIVATE nlohmann_json::nlohmann_json)
