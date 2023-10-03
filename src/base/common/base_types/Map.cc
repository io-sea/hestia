#include "Map.h"

#include "StringUtils.h"
#include <algorithm>
#include <vector>

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

bool Map::has_item(const std::string& key) const
{
    return m_data.find(key) != m_data.end();
}

void Map::add_key_prefix(const std::string& prefix)
{
    std::vector<std::string> keys;
    keys.reserve(m_data.size());
    for (const auto& [key, _] : m_data) {
        keys.push_back(key);
    }
    for (const auto& key : keys) {
        auto node  = m_data.extract(key);
        node.key() = prefix + node.key();
        m_data.insert(std::move(node));
    }
}

void Map::copy_with_prefix(
    const std::vector<std::string>& prefixes,
    Map& copy_to,
    const std::string& replacement_prefix,
    bool remove_prefix) const
{
    for (const auto& [key, value] : m_data) {

        for (const auto& prefix : prefixes) {
            if (StringUtils::starts_with(key, prefix)) {
                if (remove_prefix) {
                    copy_to.set_item(
                        replacement_prefix
                            + StringUtils::remove_prefix(key, prefix),
                        value);
                }
                else {
                    copy_to.set_item(key, value);
                }
            }
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

std::string Map::to_string(bool sort_keys) const
{
    std::string ret;
    if (sort_keys) {
        std::vector<std::string> keys(m_data.size());
        for (const auto& [key, _] : m_data) {
            keys.push_back(key);
        }
        std::sort(keys.begin(), keys.end());

        for (const auto& key : keys) {
            ret += key + " : " + get_item(key) + ", ";
        }
    }
    else {
        for (const auto& [key, value] : m_data) {
            ret += key + " : " + value + ", ";
        }
    }
    return ret;
}

}  // namespace hestia