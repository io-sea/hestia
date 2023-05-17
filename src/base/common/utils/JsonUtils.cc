#include "JsonUtils.h"

#include "FileUtils.h"

#include <fstream>
#include <nlohmann/json.hpp>

namespace hestia {
std::string JsonUtils::to_json(const Metadata& metadata)
{
    std::unordered_map<std::string, std::string> map;
    auto for_each = [&map](const std::string& key, const std::string& value) {
        map[key] = value;
    };

    metadata.for_each_item(for_each);
    nlohmann::json json = map;
    return json.dump();
}

void JsonUtils::from_json(const std::string& json_str, Metadata& metadata)
{
    const auto json = nlohmann::json::parse(json_str);
    for (const auto& [key, value] : json.items()) {
        metadata.set_item(key, value);
    }
}

std::vector<std::string> JsonUtils::get_values(
    const std::string& json_str, const std::vector<std::string>& keys)
{
    std::vector<std::string> vals;
    const auto json = nlohmann::json::parse(json_str);
    for (const auto& key : keys) {
        if (json.contains(key)) {
            vals.push_back(json[key]);
        }
        else {
            vals.push_back("");
        }
    }
    return vals;
}

void JsonUtils::get_value(
    const std::filesystem::path& path,
    const std::string& key,
    std::string& value)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    if (file_content.contains(key)) {
        value = file_content[key];
    }
}

bool JsonUtils::has_key(
    const std::filesystem::path& path, const std::string& key)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    return file_content.contains(key);
}

void JsonUtils::remove_key(
    const std::filesystem::path& path, const std::string& key)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    file_content.erase(key);

    std::ofstream out_file;
    out_file.open(path);
    out_file << file_content;
}

void JsonUtils::set_value(
    const std::filesystem::path& path,
    const std::string& key,
    const std::string& value)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    file_content[key] = value;

    std::ofstream out_file;
    out_file.open(path);
    out_file << file_content;
}

void JsonUtils::read(
    const std::filesystem::path& path,
    Metadata& metadata,
    const std::vector<std::string>& keys)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;

    for (const auto& [key, value] : file_content.items()) {
        if (keys.empty()) {
            metadata.set_item(key, value);
        }
        else if (std::find(keys.begin(), keys.end(), key) != keys.end()) {
            metadata.set_item(key, value);
        }
    }
}

void JsonUtils::write(
    const std::filesystem::path& path, const Metadata& metadata, bool merge)
{
    nlohmann::json existing_content;
    if (merge && std::filesystem::is_regular_file(path)) {
        std::ifstream read_file(path);
        read_file >> existing_content;
    }

    nlohmann::json new_content;
    auto each_item =
        [&new_content](const std::string& key, const std::string& value) {
            new_content.emplace(key, value);
        };
    metadata.for_each_item(each_item);

    existing_content.merge_patch(new_content);

    FileUtils::create_if_not_existing(path);

    std::ofstream out_file;
    out_file.open(path);
    out_file << existing_content;
}
}  // namespace hestia