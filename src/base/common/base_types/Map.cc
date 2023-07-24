#include "Map.h"

#include "StringUtils.h"

namespace hestia {

std::string Map::get_item(const std::string& key) const
{
    if (auto iter = m_data.find(key); iter != m_data.end()) {
        return iter->second;
    }
    return {};
}

bool Map::has_key_and_value(const KeyValuePair& query) const
{
    return get_item(query.first) == query.second;
}

void Map::add_key_prefix(const std::string& prefix)
{
    std::vector<std::string> replace_keys;
    for (const auto& [key, value] : m_data) {
        m_data[prefix + key] = value;
        replace_keys.push_back(key);
    }
    for (const auto& key : replace_keys) {
        m_data.erase(key);
    }
}

void Map::copy_with_prefix(const std::string& prefix, Map& copy_to) const
{
    for (const auto& [key, value] : m_data) {
        if (StringUtils::starts_with(key, prefix)) {
            copy_to.set_item(StringUtils::remove_prefix(key, prefix), value);
        }
    }
}

bool Map::empty() const
{
    return m_data.empty();
}

const std::unordered_map<std::string, std::string>& Map::data() const
{
    return m_data;
}

bool Map::key_contains(
    const std::string& key, const std::string& search_term) const
{
    return key.rfind(search_term, 0) != 0;
}

void Map::merge(const Map& other)
{
    m_data.insert(other.m_data.begin(), other.m_data.end());
}

void Map::set_item(const std::string& key, const std::string& item)
{
    m_data[key] = item;
}

void Map::for_each_item(onItem func) const
{

    for (const auto& entry : m_data) {
        func(entry.first, entry.second);
    }
}

std::string Map::to_string() const
{
    std::string ret;
    for (const auto& [key, value] : m_data) {
        ret += key + " : " + value + ", ";
    }
    return ret;
}

}  // namespace hestia