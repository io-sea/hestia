#include <algorithm>
#include <fstream>
#include <string>
#include <unordered_map>

#include "disk.h"

bool hestia::kv::Disk::object_exists(const struct hsm_uint& oid)
{
    return false;
}

int hestia::kv::Disk::put_meta_data(const struct hsm_obj& obj)
{
    std::ofstream file(
        std::to_string(obj.oid.higher) + std::to_string(obj.oid.lower) + ".md",
        std::ios_base::app);

    std::for_each(
        obj.meta_data.begin(), obj.meta_data.end(), [&file](const auto& md) {
            file << md.first << " ; " << md.second << '\n';
        });

    return 0;
}

int hestia::kv::Disk::put_meta_data(
    const struct hsm_uint& oid, const nlohmann::json& attrs)
{
    auto attrs_map = attrs.get<std::unordered_map<std::string, std::string>>();

    std::ofstream file(
        std::to_string(oid.higher) + std::to_string(oid.lower) + ".md",
        std::ios_base::app);

    std::for_each(attrs_map.begin(), attrs_map.end(), [&file](const auto& md) {
        file << md.first << " ; " << md.second << '\n';
    });

    return 0;
}

int hestia::kv::Disk::get_meta_data(struct hsm_obj& obj)
{
    std::ifstream file(
        std::to_string(obj.oid.higher) + std::to_string(obj.oid.lower) + ".md");

    std::string line;
    std::string delim = ";";

    std::unordered_map<std::string, std::string> temp_md;
    while (std::getline(file, line)) {
        // split into the key and value using the delim ;
        // turn them into an unordered map
        int pos           = line.find(delim);
        std::string key   = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        temp_md.insert({key, value});
    }

    temp_md.merge(obj.meta_data);

    std::swap(obj.meta_data, temp_md);

    return 0;
}
