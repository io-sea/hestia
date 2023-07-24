#include "InMemoryKeyValueStoreClient.h"

#include "StringUtils.h"

#include <iterator>
#include <set>
#include <sstream>

#include "Logger.h"

namespace hestia {
InMemoryKeyValueStoreClient::InMemoryKeyValueStoreClient() {}

void InMemoryKeyValueStoreClient::initialize(
    const std::string&, const Dictionary&)
{
}

std::string InMemoryKeyValueStoreClient::dump() const
{
    std::stringstream sstr;
    sstr << "Strings db: \n";
    for (const auto& [key, value] : m_string_db) {
        sstr << key << " : " << value << "\n";
    }

    sstr << "Sets db: \n";
    for (const auto& [key, value] : m_set_db) {
        std::stringstream set_sstr;
        for (const auto& set_item : value) {
            set_sstr << set_item << ";";
        }
        sstr << key << " : " << set_sstr.str() << "\n";
    }
    return sstr.str();
}

void InMemoryKeyValueStoreClient::string_get(
    const std::vector<std::string>& keys,
    std::vector<std::string>& values) const
{
    for (const auto& key : keys) {
        std::string value;
        if (auto iter = m_string_db.find(key); iter != m_string_db.end()) {
            value = iter->second;
        }
        values.push_back(value);
    }
}

void InMemoryKeyValueStoreClient::string_set(
    const std::vector<KeyValuePair>& kv_pairs) const
{
    for (const auto& kv_pair : kv_pairs) {
        m_string_db[kv_pair.first] = kv_pair.second;
    }
}

void InMemoryKeyValueStoreClient::string_remove(
    const std::vector<std::string>& keys) const
{
    for (const auto& key : keys) {
        m_string_db.erase(key);
    }
}

void InMemoryKeyValueStoreClient::string_exists(
    const std::vector<std::string>& keys, std::vector<bool>& found) const
{
    for (const auto& key : keys) {
        found.push_back(m_string_db.find(key) != m_string_db.end());
    }
}

void InMemoryKeyValueStoreClient::set_add(const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        m_set_db[entry.first].insert(entry.second);
    }
}

void InMemoryKeyValueStoreClient::set_list(
    const std::vector<std::string>& keys,
    std::vector<std::vector<std::string>>& total_values) const
{
    for (const auto& key : keys) {
        std::vector<std::string> values;
        if (auto iter = m_set_db.find(key); iter != m_set_db.end()) {
            std::copy(
                iter->second.begin(), iter->second.end(),
                std::back_inserter(values));
        }
        total_values.push_back(values);
    }
}

void InMemoryKeyValueStoreClient::set_remove(
    const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        m_set_db[entry.first].erase(entry.second);
    }
}

}  // namespace hestia