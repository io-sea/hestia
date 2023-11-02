#include "JsonUtils.h"

#include "ErrorUtils.h"
#include "FileUtils.h"
#include "Logger.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <nlohmann/json.hpp>

namespace hestia {
std::string JsonUtils::to_json(const Map& metadata)
{
    std::unordered_map<std::string, std::string> map;
    auto for_each = [&map](const std::string& key, const std::string& value) {
        map[key] = value;
    };

    metadata.for_each_item(for_each);
    nlohmann::json json = map;
    return json.dump();
}

void JsonUtils::from_json(const std::string& json_str, Map& metadata)
{
    const auto json = nlohmann::json::parse(json_str);
    for (const auto& [key, value] : json.items()) {
        metadata.set_item(key, value);
    }
}

void JsonUtils::read_values(
    const std::string& buffer,
    const std::string& path,
    std::vector<std::string>& values)
{
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(buffer);
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error in json parsing: " << e.what());
        throw e;
    }

    if (json.is_array()) {
        for (const auto& [key, value] : json.items()) {
            if (value.is_object()) {
                for (const auto& [obj_key, obj_value] : json.items()) {
                    if (obj_key == path && obj_value.is_string()) {
                        values.push_back(obj_value);
                    }
                }
            }
        }
    }
    else if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            if (key == path && value.is_string()) {
                values.push_back(value);
            }
        }
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

bool JsonUtils::get_value(
    const std::filesystem::path& path,
    const std::string& key,
    std::string& value)
{
    if (!std::filesystem::exists(path)) {
        return false;
    }

    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    if (file_content.contains(key)) {

        if (!file_content[key].is_null()) {
            if (file_content[key].is_string()) {
                value = file_content[key];
            }
            else {
                value = file_content[key].dump();
            }
        }
        return true;
    }
    else {
        return false;
    }
}

void JsonUtils::get_values(
    const std::filesystem::path& path,
    const std::vector<std::string>& keys,
    std::vector<std::string>& values)
{
    if (!std::filesystem::exists(path)) {
        return;
    }

    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    for (const auto& key : keys) {
        if (file_content.contains(key)) {
            const auto content = file_content[key];
            if (content.is_string()) {
                values.push_back(content);
            }
            else {
                values.push_back(content.dump());
            }
        }
    }
}

bool JsonUtils::has_key(
    const std::filesystem::path& path, const std::string& key)
{
    if (!std::filesystem::is_regular_file(path)) {
        return false;
    }

    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    return file_content.contains(key);
}

void JsonUtils::has_keys(
    const std::filesystem::path& path,
    const std::vector<std::string>& keys,
    std::vector<bool>& found)
{
    if (!std::filesystem::is_regular_file(path)) {
        found = std::vector<bool>(keys.size(), false);
        return;
    }

    std::ifstream read_file(path);

    nlohmann::json file_content;

    try {
        read_file >> file_content;
    }
    catch (const std::exception& e) {
        const auto msg =
            SOURCE_LOC() + " | Exception checking keys: " + e.what();
        LOG_ERROR(msg);
        throw std::runtime_error(msg);
    }

    read_file.close();

    for (const auto& key : keys) {
        found.push_back(file_content.contains(key));
    }
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

void JsonUtils::remove_keys(
    const std::filesystem::path& path, const std::vector<std::string>& keys)
{
    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
    read_file.close();

    for (const auto& key : keys) {
        file_content.erase(key);
    }

    std::ofstream out_file;
    out_file.open(path);
    out_file << file_content;
}

void JsonUtils::set_value(
    const std::filesystem::path& path,
    const std::string& key,
    const std::string& value)
{
    nlohmann::json file_content;

    if (std::filesystem::is_regular_file(path)) {
        std::ifstream read_file(path);
        read_file >> file_content;
        read_file.close();
    }

    try {
        file_content[key] = nlohmann::json::parse(value);
    }
    catch (...) {
        file_content[key] = value;
    }

    std::ofstream out_file(path);
    out_file << file_content;
}

void JsonUtils::set_values(
    const std::filesystem::path& path,
    const std::vector<KeyValuePair>& kv_pairs)
{
    nlohmann::json file_content;

    if (std::filesystem::is_regular_file(path)) {
        std::ifstream read_file(path);
        read_file >> file_content;
        read_file.close();
    }

    for (const auto& [key, value] : kv_pairs) {
        try {
            file_content[key] = nlohmann::json::parse(value);
        }
        catch (...) {
            file_content[key] = value;
        }
    }

    std::ofstream out_file(path);
    out_file << file_content;
}

void JsonUtils::read(
    const std::filesystem::path& path,
    Map& metadata,
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
    const std::filesystem::path& path, const Map& metadata, bool merge)
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

    std::ofstream out_file(path);
    out_file << existing_content;
}
}  // namespace hestia