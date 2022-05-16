#include <filesystem>
#include <fstream>

#include "disk.h"

bool hestia::kv::Disk::object_exists(const struct hsm_uint& oid)
{
    return std::filesystem::exists(get_filename_from_oid(oid));
}

int hestia::kv::Disk::put_meta_data(const struct hsm_obj& obj)
{
    return put_meta_data(obj.oid, obj.meta_data);
}

int hestia::kv::Disk::put_meta_data(
    const struct hsm_uint& oid, const nlohmann::json& attrs)
{
    auto metadata = attrs;

    if (object_exists(oid)) {
        std::ifstream read_file(get_filename_from_oid(oid));

        /* we need to overwrite potential duplicates */
        nlohmann::json old_metadata;
        read_file >> old_metadata;

        metadata.merge_patch(old_metadata);
    }

    std::ofstream file(get_filename_from_oid(oid));

    file << metadata;

    return 0;
}

int hestia::kv::Disk::get_meta_data(struct hsm_obj& obj)
{
    std::ifstream file(get_filename_from_oid(obj.oid));

    nlohmann::json temp_md;

    file >> temp_md;

    obj.meta_data.merge_patch(temp_md);

    return 0;
}

int hestia::kv::Disk::get_meta_data(
    const struct hsm_uint& oid, nlohmann::json& attrs)
{
    std::ifstream file(get_filename_from_oid(oid));

    file >> attrs;

    return 0;
}

int hestia::kv::Disk::get_meta_data(
    const struct hsm_uint& oid, char* attr_keys, nlohmann::json& attrs)
{
    if (!object_exists(oid)) {
        return -1;
    }

    std::ifstream read_file(get_filename_from_oid(oid));

    nlohmann::json metadata;
    read_file >> metadata;

    char* key;
    key = strtok(attr_keys, &m_delim);

    while (key != NULL) {
        if (!metadata.contains(key)) {
            return -1;
        }
        attrs[key] = metadata[key];

        key = strtok(NULL, &m_delim);
    }

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
    std::vector<struct hsm_uint> oid_list;
    for (const auto& file : std::filesystem::directory_iterator{"./"}) {
        auto filename               = file.path().filename().string();
        const std::string extension = ".meta";

        if (filename.size() > extension.size()
            && filename.substr(
                   filename.size() - extension.size(), extension.size())
                   == extension) {
            std::ifstream metadata_file(filename);
            nlohmann::json metadata;

            /* extract all metadata entries */
            metadata_file >> metadata;

            if (metadata["tier"] == tier) {
                const auto int_split = filename.find_first_of('-');
                const std::uint64_t higher =
                    std::stoull(filename.substr(0, int_split));
                const std::uint64_t lower = std::stoull(filename.substr(
                    int_split + 1,
                    filename.size() - int_split - 1 - extension.size()));
                oid_list.push_back(hsm_uint(higher, lower));
            }
        }
    }

    return oid_list;
}
