#include <algorithm>
#include <filesystem>
#include <fstream>

#include "disk.h"

bool hestia::kv::Disk::object_exists(const struct hsm_uint& oid)
{
    return std::filesystem::exists(get_filename_from_oid(oid));
}

int hestia::kv::Disk::put_meta_data(const struct hsm_obj& obj)
{
    std::ofstream file(get_filename_from_oid(obj.oid), std::ios_base::app);

    std::for_each(
        obj.meta_data.begin(), obj.meta_data.end(),
        [&file, this](const auto& md) {
            file << md.first << m_delim << md.second << '\n';
        });

    return 0;
}

int hestia::kv::Disk::put_meta_data(
    const struct hsm_uint& oid, const nlohmann::json& attrs)
{
    auto attrs_map = attrs.get<std::unordered_map<std::string, std::string>>();

    std::ofstream file(get_filename_from_oid(oid), std::ios_base::app);

    std::for_each(
        attrs_map.begin(), attrs_map.end(), [&file, this](const auto& md) {
            file << md.first << m_delim << md.second << '\n';
        });

    return 0;
}

int hestia::kv::Disk::get_meta_data(struct hsm_obj& obj)
{
    std::ifstream file(get_filename_from_oid(obj.oid));

    std::unordered_map<std::string, std::string> temp_md;

    std::string key;
    std::string value;
    while (std::getline(file, key, m_delim) && std::getline(file, value)) {
        temp_md[key] = value;
    }

    temp_md.merge(obj.meta_data);

    std::swap(obj.meta_data, temp_md);

    return 0;
}

int hestia::kv::Disk::remove(const struct hsm_uint& oid)
{
    return static_cast<int>(
        std::filesystem::remove(get_filename_from_oid(oid)));
}

std::vector<struct hestia::hsm_uint> hestia::kv::Disk::list(
    const std::uint8_t tier)
{
    const std::string tier_string = std::to_string(tier);
    std::vector<struct hsm_uint> oid_list;
    for (const auto& file : std::filesystem::directory_iterator{"./"}) {
        auto filename = file.path().filename().string();

        if (filename.substr(filename.size() - 3, 3) == ".md") {
            std::ifstream metadata_file(filename);
            std::unordered_map<std::string, std::string> metadata;

            /* extract all metadata entries */
            std::string key;
            std::string value;
            while (std::getline(metadata_file, key, m_delim)
                   && std::getline(metadata_file, value)) {
                metadata[key] = value;
            }

            if (metadata["tier"] == tier_string) {
                const auto int_split = filename.find_first_of('-');
                const std::uint64_t higher =
                    std::stoull(filename.substr(0, int_split));
                const std::uint64_t lower = std::stoull(filename.substr(
                    int_split + 1, filename.size() - int_split - 4));
                oid_list.push_back(hsm_uint(higher, lower));
            }
        }
    }

    return oid_list;
}
