add_library(kv_disk
    disk.cc
)

target_compile_features(kv_disk PUBLIC cxx_std_17)

