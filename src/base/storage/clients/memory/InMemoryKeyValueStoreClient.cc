#include "InMemoryKeyValueStoreClient.h"

#include "JsonUtils.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include <iterator>
#include <set>
#include <sstream>

#include "Logger.h"

namespace hestia {
InMemoryKeyValueStoreClient::InMemoryKeyValueStoreClient() {}

void InMemoryKeyValueStoreClient::initialize(const Metadata& config_data)
{
    (void)config_data;
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
            set_sstr << UuidUtils::to_string(set_item) << ";";
        }
        sstr << key << " : " << set_sstr.str() << "\n";
    }
    return sstr.str();
}

void InMemoryKeyValueStoreClient::string_get(
    const std::string& key, std::string& value) const
{
    if (auto iter = m_string_db.find(key); iter != m_string_db.end()) {
        value = iter->second;
    }
}

void InMemoryKeyValueStoreClient::string_multi_get(
    const std::vector<std::string>& keys,
    std::vector<std::string>& values) const
{
    for (const auto& key : keys) {
        std::string value;
        string_get(key, value);
        values.push_back(value);
    }
}

void InMemoryKeyValueStoreClient::string_set(
    const std::string& key, const std::string& value) const
{
    LOG_INFO("Setting value: " << key << " | " << value);
    m_string_db[key] = value;
}

void InMemoryKeyValueStoreClient::string_remove(const std::string& key) const
{
    m_string_db.erase(key);
}

bool InMemoryKeyValueStoreClient::string_exists(const std::string& key) const
{
    return m_string_db.find(key) != m_string_db.end();
}

void InMemoryKeyValueStoreClient::set_add(
    const std::string& key, const Uuid& value) const
{
    m_set_db[key].insert(value);
}

void InMemoryKeyValueStoreClient::set_list(
    const std::string& key, std::vector<Uuid>& values) const
{
    if (auto iter = m_set_db.find(key); iter != m_set_db.end()) {
        std::copy(
            iter->second.begin(), iter->second.end(),
            std::back_inserter(values));
    }
}

void InMemoryKeyValueStoreClient::set_remove(
    const std::string& key, const Uuid& value) const
{
    m_set_db[key].erase(value);
}

}  // namespace hestia