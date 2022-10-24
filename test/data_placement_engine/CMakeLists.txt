add_library(dpe
    eejit.cc
)

set_target_properties(dpe PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_compile_features(dpe PUBLIC cxx_std_17)
target_link_libraries(dpe PRIVATE nlohmann_json::nlohmann_json)
