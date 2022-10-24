add_library(obj_disk
    disk.cc
)

set_target_properties(obj_disk PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(obj_disk PUBLIC cxx_std_17)
target_link_libraries(obj_disk PRIVATE nlohmann_json::nlohmann_json)
