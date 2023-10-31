#include "JsonDocument.h"

#include "Logger.h"

#include "ErrorUtils.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace hestia {

class JsonDocumentImpl {
  public:
    void append(const nlohmann::json& parsed)
    {
        if (m_json.is_array()) {
            if (parsed.is_array()) {
                for (const auto& [key, value] : parsed.items()) {
                    m_json.push_back(value);
                }
            }
            else {
                m_json.push_back(parsed);
            }
        }
        else if (!parsed.is_array()) {
            m_json = {m_json, parsed};
        }
    }

    nlohmann::json m_json;
};

void on_parse_error(
    const std::string& source_loc,
    const std::exception& e,
    const std::string& payload)
{
    const auto msg = source_loc + " | " + "Error in json parsing: " + e.what()
                     + " | Payload: \n" + payload;
    LOG_ERROR(msg);
    throw e;
}

JsonDocument::JsonDocument(const std::string& raw) :
    m_impl(std::make_unique<JsonDocumentImpl>())
{
    load(raw);
}

JsonDocument::JsonDocument(const JsonDocument& other)
{
    m_impl = std::make_unique<JsonDocumentImpl>(*other.m_impl);
}

JsonDocument::JsonDocument(const Dictionary& dict) :
    m_impl(std::make_unique<JsonDocumentImpl>())
{
    load(dict);
}

JsonDocument::JsonDocument(const std::vector<std::string>& raw) :
    m_impl(std::make_unique<JsonDocumentImpl>())
{
    try {
        m_impl->m_json = nlohmann::json::array();
        for (const auto& item : raw) {
            m_impl->m_json.push_back(nlohmann::json::parse(item));
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR("Error in json parsing: " << e.what());
        throw e;
    }
}

JsonDocument::Ptr JsonDocument::create(const std::string& raw)
{
    return std::make_unique<JsonDocument>(raw);
}

JsonDocument::Ptr JsonDocument::create(const Dictionary& dict)
{
    return std::make_unique<JsonDocument>(dict);
}

JsonDocument::~JsonDocument() {}

std::size_t JsonDocument::get_size() const
{
    if (m_impl->m_json.is_array()) {
        return m_impl->m_json.size();
    }
    else {
        return 1;
    }
}

void JsonDocument::get_values(
    const SearchExpression& search, std::vector<std::string>& values) const
{
    if (m_impl->m_json.is_array()) {
        for (const auto& [key, value] : m_impl->m_json.items()) {
            if (value.is_object()) {
                for (const auto& [obj_key, obj_value] : value.items()) {
                    if (obj_key == search.m_path && obj_value.is_string()) {
                        values.push_back(obj_value);
                    }
                }
            }
        }
    }
    else if (m_impl->m_json.is_object()) {
        for (const auto& [key, value] : m_impl->m_json.items()) {
            if (key == search.m_path && value.is_string()) {
                values.push_back(value);
            }
        }
    }
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
            auto scalar_type = dict.get_scalar_type();
            if (scalar_type == Dictionary::ScalarType::INT) {
                json = std::stoull(dict.get_scalar());
            }
            else if (scalar_type == Dictionary::ScalarType::FLOAT) {
                json = std::stod(dict.get_scalar());
            }
            else if (scalar_type == Dictionary::ScalarType::BOOL) {
                json = dict.get_scalar() == "true";
            }
            else {
                json = dict.get_scalar();
            }
        }
        else if (!is_excluded(key, exclude_keys)) {

            auto scalar_type = dict.get_scalar_type();
            if (scalar_type == Dictionary::ScalarType::INT) {
                json[key] = std::stoull(dict.get_scalar());
            }
            else if (scalar_type == Dictionary::ScalarType::FLOAT) {
                json[key] = std::stod(dict.get_scalar());
            }
            else if (scalar_type == Dictionary::ScalarType::BOOL) {
                json[key] = dict.get_scalar() == "true";
            }
            else {
                json[key] = dict.get_scalar();
            }
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

const JsonDocument& JsonDocument::load(
    const Dictionary& dict, const std::vector<std::string>& exclude_keys)
{
    to_json_internal(m_impl->m_json, dict, exclude_keys);
    if (m_impl->m_json.is_null() && dict.is_sequence()) {
        m_impl->m_json = nlohmann::json::array();
    }
    return *this;
}

const JsonDocument& JsonDocument::load(const std::string& body)
{
    if (!body.empty()) {
        try {
            m_impl->m_json = nlohmann::json::parse(body);
        }
        catch (const std::exception& e) {
            on_parse_error(SOURCE_LOC(), e, body);
        }
    }
    return *this;
}

void JsonDocument::append(const std::string& body)
{
    if (body.empty()) {
        return;
    }

    if (is_empty()) {
        load(body);
        return;
    }

    nlohmann::json parsed;
    try {
        parsed = nlohmann::json::parse(body);
    }
    catch (const std::exception& e) {
        on_parse_error(SOURCE_LOC(), e, body);
    }

    try {
        m_impl->append(parsed);
    }
    catch (const std::exception& e) {
        on_parse_error(SOURCE_LOC(), e, {});
    }
}

void JsonDocument::append(
    const Dictionary& dict, const std::vector<std::string>& exclude_keys)
{
    nlohmann::json parsed;
    to_json_internal(parsed, dict, exclude_keys);
    m_impl->append(parsed);
}

bool JsonDocument::is_empty() const
{
    return m_impl->m_json.empty();
}

std::string JsonDocument::to_string(
    const Index& index, const FormatSpec& format) const
{
    std::string ret;
    write(ret, index, format);
    return ret;
}

void dump(
    const nlohmann::json& json,
    std::string& buffer,
    const JsonDocument::FormatSpec& format)
{
    if (format.m_indent) {
        buffer = json.dump(format.m_indent_size);
    }
    else {
        buffer = json.dump();
    }
}

void JsonDocument::write(
    std::string& buffer, const Index& index, const FormatSpec& format) const
{
    if (m_impl->m_json.is_null()) {
        m_impl->m_json = nlohmann::json::array();
    }

    if (m_impl->m_json.is_array()) {
        if (index.is_set()) {
            if (index.is_set() && index.value() > m_impl->m_json.size()) {
                throw std::range_error(
                    SOURCE_LOC() + " | Out of range access attempted.");
            }
            dump(m_impl->m_json, buffer, format);
        }
        else {
            if (m_impl->m_json.size() == 1 && format.m_collapse_single) {
                dump(m_impl->m_json[0], buffer, format);
            }
            else {
                dump(m_impl->m_json, buffer, format);
            }
        }
    }
    else {
        if (index.is_set() && index.value() > 0) {
            throw std::range_error(
                SOURCE_LOC() + " | Out of range access attempted.");
        }
        dump(m_impl->m_json, buffer, format);
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

        if (j.is_boolean()) {
            scalar_dict->set_scalar(
                j ? "true" : "false", Dictionary::ScalarType::BOOL);
        }
        else if (j.is_number_integer()) {
            scalar_dict->set_scalar(
                std::to_string(j.get<unsigned long long>()),
                Dictionary::ScalarType::INT);
        }
        else if (j.is_number_float()) {
            scalar_dict->set_scalar(
                std::to_string(j.get<float>()), Dictionary::ScalarType::FLOAT);
        }
        else {
            scalar_dict->set_scalar(
                j.get<std::string>(), Dictionary::ScalarType::STRING);
        }
        if (parent_key.empty()) {
            dict.add_sequence_item(std::move(scalar_dict));
        }
        else {
            dict.set_map_item(parent_key, std::move(scalar_dict));
        }
    }
}

void JsonDocument::write(
    Dictionary& dict, const Index& index, const FormatSpec& format)
{
    if (m_impl->m_json.is_array()) {
        if (index.is_set()) {
            if (index.value() >= m_impl->m_json.size()) {
                throw std::range_error(
                    SOURCE_LOC() + " | Out of range access attempted.");
            }
            else {
                from_json_internal(
                    m_impl->m_json[index.value()], dict, format.m_exclude_keys);
            }
        }
        else {
            dict.set_type(Dictionary::Type::SEQUENCE);
            for (const auto& [key, value] : m_impl->m_json.items()) {
                if (is_excluded(key, format.m_exclude_keys)) {
                    continue;
                }
                from_json_internal(value, dict, format.m_exclude_keys);
            }
        }
    }
    else {
        if (index.is_set() && index.value() > 0) {
            throw std::range_error(
                SOURCE_LOC() + " | Out of range access attempted.");
        }

        if (m_impl->m_json.is_object()) {
            for (const auto& [key, value] : m_impl->m_json.items()) {
                if (is_excluded(key, format.m_exclude_keys)) {
                    continue;
                }
                from_json_internal(value, dict, format.m_exclude_keys, key);
            }
        }
        else {
            dict.set_type(Dictionary::Type::SCALAR);
            if (m_impl->m_json.is_boolean()) {
                dict.set_scalar(
                    m_impl->m_json ? "true" : "false",
                    Dictionary::ScalarType::BOOL);
            }
            else if (m_impl->m_json.is_number_integer()) {
                dict.set_scalar(
                    std::to_string(m_impl->m_json.get<unsigned long long>()),
                    Dictionary::ScalarType::INT);
            }
            else if (m_impl->m_json.is_number_float()) {
                dict.set_scalar(
                    std::to_string(m_impl->m_json.get<float>()),
                    Dictionary::ScalarType::FLOAT);
            }
            else {
                dict.set_scalar(
                    m_impl->m_json.get<std::string>(),
                    Dictionary::ScalarType::STRING);
            }
        }
    }
}

}  // namespace hestia