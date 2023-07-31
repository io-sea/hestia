#include "JsonUtils.h"

#include "FileUtils.h"

#include <algorithm>
#include <filesystem>
#include <fstream>

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

std::string JsonUtils::to_json(
    const std::vector<Map>& metadata, const std::string& key)
{
    std::vector<std::unordered_map<std::string, std::string>> data;
    for (const auto& md : metadata) {
        data.push_back(md.data());
    }

    nlohmann::json json;
    if (key.empty()) {
        json = data;
    }
    else {
        json[key] = data;
    }
    return json.dump();
}

static bool is_excluded(
    const std::string& key, const std::vector<std::string>& exclude_keys)
{
    return std::find(exclude_keys.begin(), exclude_keys.end(), key)
           != exclude_keys.end();
}

void to_json_internal(
    nlohmann::json& json,
    const Dictionary& dict,
    const std::vector<std::string>& exclude_keys,
    const std::string& key = {})
{
    if (dict.get_type() == Dictionary::Type::SCALAR) {
        if (key.empty()) {
            json = dict.get_scalar();
        }
        else if (!is_excluded(key, exclude_keys)) {
            json[key] = dict.get_scalar();
        }
        return;
    }
    else if (dict.get_type() == Dictionary::Type::SEQUENCE) {
        std::vector<nlohmann::json> dict_items;
        for (const auto& item : dict.get_sequence()) {
            nlohmann::json dict_json;
            to_json_internal(dict_json, *item, exclude_keys);
            dict_items.push_back(dict_json);
        }
        if (key.empty()) {
            json = dict_items;
        }
        else if (!is_excluded(key, exclude_keys)) {
            json[key] = dict_items;
        }
        return;
    }
    else if (dict.get_type() == Dictionary::Type::MAP) {
        for (const auto& [key, dict_item] : dict.get_map()) {

            if (is_excluded(key, exclude_keys)) {
                continue;
            }

            nlohmann::json dict_json;
            to_json_internal(dict_json, *dict_item, exclude_keys);
            if (dict_json.is_null()) {
                dict_json = std::unordered_map<std::string, std::string>();
            }
            json[key] = dict_json;
        }
        return;
    }
}

void JsonUtils::to_json(
    const Dictionary& dict,
    std::string& output,
    const std::vector<std::string>& exclude_keys,
    unsigned indent)
{
    nlohmann::json json;
    to_json_internal(json, dict, exclude_keys);

    if (indent > 0) {
        output = json.dump(indent);
    }
    else {
        output = json.dump();
    }
}

void from_json_internal(
    const nlohmann::json& j,
    Dictionary& dict,
    const std::vector<std::string>& exclude_keys,
    const std::string& parent_key = {})
{
    if (j.is_array()) {
        auto array_dict =
            std::make_unique<Dictionary>(Dictionary::Type::SEQUENCE);
        for (const auto& [key, value] : j.items()) {
            if (is_excluded(key, exclude_keys)) {
                continue;
            }
            from_json_internal(value, *array_dict, exclude_keys);
        }
        dict.set_map_item(parent_key, std::move(array_dict));
    }
    else if (j.is_object()) {
        if (dict.get_type() == Dictionary::Type::SEQUENCE) {
            auto map_dict = std::make_unique<Dictionary>();
            for (const auto& [key, value] : j.items()) {
                if (is_excluded(key, exclude_keys)) {
                    continue;
                }
                from_json_internal(value, *map_dict, exclude_keys, key);
            }
            dict.add_sequence_item(std::move(map_dict));
        }
        else if (parent_key.empty()) {
            for (const auto& [key, value] : j.items()) {
                if (is_excluded(key, exclude_keys)) {
                    continue;
                }
                from_json_internal(value, dict, exclude_keys, key);
            }
        }
        else {
            auto map_dict = std::make_unique<Dictionary>();
            for (const auto& [key, value] : j.items()) {
                if (is_excluded(key, exclude_keys)) {
                    continue;
                }
                from_json_internal(value, *map_dict, exclude_keys, key);
            }
            dict.set_map_item(parent_key, std::move(map_dict));
        }
    }
    else {
        auto scalar_dict =
            std::make_unique<Dictionary>(Dictionary::Type::SCALAR);
        scalar_dict->set_scalar(j);
        dict.set_map_item(parent_key, std::move(scalar_dict));
    }
}

void JsonUtils::from_json(
    const std::string& str,
    Dictionary& dict,
    const std::vector<std::string>& exclude_keys)
{
    if (str.empty()) {
        return;
    }
    const auto json = nlohmann::json::parse(str);

    if (json.is_array()) {
        dict.set_type(Dictionary::Type::SEQUENCE);

        for (const auto& [key, value] : json.items()) {
            if (is_excluded(key, exclude_keys)) {
                continue;
            }
            from_json_internal(value, dict, exclude_keys);
        }
    }
    else if (json.is_object()) {
        for (const auto& [key, value] : json.items()) {
            if (is_excluded(key, exclude_keys)) {
                continue;
            }
            from_json_internal(value, dict, exclude_keys, key);
        }
    }
    else {
        dict.set_type(Dictionary::Type::SCALAR);
        dict.set_scalar(json);
    }
}

void JsonUtils::from_json(const std::string& json_str, Map& metadata)
{
    const auto json = nlohmann::json::parse(json_str);
    for (const auto& [key, value] : json.items()) {
        metadata.set_item(key, value);
    }
}

void JsonUtils::from_json(
    const std::string& json_str, std::vector<Map>& metadata)
{
    const auto json = nlohmann::json::parse(json_str);
    for (const auto& [key, value] : json.items()) {
        Map data;
        from_json(value, data);
        metadata.push_back(data);
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
    }

    std::ifstream read_file(path);

    nlohmann::json file_content;
    read_file >> file_content;
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