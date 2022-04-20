#include <algorithm>
#include <fstream>
#include <unordered_map>

#include "disk.h"

bool hestia::kv::Disk::object_exists(const struct hsm_uint& oid)
{
    return false;
}

int hestia::kv::Disk::put_meta_data(const struct hsm_obj& obj)
{
    std::ofstream file(
        std::to_string(obj.oid.higher) + std::to_string(obj.oid.lower) + ".md");

    std::for_each(
        obj.meta_data.begin(), obj.meta_data.end(), [&file](const auto& md) {
            file << md.first << " " << md.second << '\n';
        });

    return 0;
}
