add_library(kv_disk
    disk.cc
)

set_target_properties(kv_disk PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(kv_disk PUBLIC cxx_std_17)
target_link_libraries(kv_disk PRIVATE nlohmann_json::nlohmann_json)
