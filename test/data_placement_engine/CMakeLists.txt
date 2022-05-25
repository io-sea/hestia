add_library(dpe
    eejit.cc
)

target_compile_features(dpe PUBLIC cxx_std_17)
target_link_libraries(dpe PRIVATE nlohmann_json::nlohmann_json)
