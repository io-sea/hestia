#include "FileKeyValueStoreClient.h"

#include "File.h"
#include "FileUtils.h"
#include "JsonUtils.h"
#include "StringUtils.h"

#include <fstream>
#include <iterator>
#include <set>

#include "Logger.h"

namespace hestia {
FileKeyValueStoreClient::FileKeyValueStoreClient() {}

FileKeyValueStoreClient::~FileKeyValueStoreClient()
{
    LOG_INFO("Destroyed FileKeyValueStoreClient");
}

void FileKeyValueStoreClient::initialize(
    const std::string& cache_path, const Dictionary& config_data)
{
    FileKeyValueStoreClientConfig config;
    config.deserialize(config_data);
    do_initialize(cache_path, config);
}

void FileKeyValueStoreClient::do_initialize(
    const std::string& cache_path, const FileKeyValueStoreClientConfig& config)
{
    m_store = std::filesystem::path(config.m_root.get_value());
    if (m_store.is_relative()) {
        m_store = std::filesystem::path(cache_path) / m_store;
    }
    LOG_INFO("Initializing at: " + m_store.string());
}

void FileKeyValueStoreClient::string_get(
    const std::vector<std::string>& keys,
    std::vector<std::string>& values) const
{
    JsonUtils::get_values(m_store / m_db_name, keys, values);
}

void FileKeyValueStoreClient::string_set(
    const std::vector<KeyValuePair>& kv_pairs) const
{
    const auto path = m_store / m_db_name;
    FileUtils::create_if_not_existing(path);
    JsonUtils::set_values(path, kv_pairs);
}

void FileKeyValueStoreClient::string_remove(
    const std::vector<std::string>& keys) const
{
    JsonUtils::remove_keys(m_store / m_db_name, keys);
}

void FileKeyValueStoreClient::string_exists(
    const std::vector<std::string>& keys, std::vector<bool>& found) const
{
    return JsonUtils::has_keys(m_store / m_db_name, keys, found);
}

void FileKeyValueStoreClient::set_add(const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        const auto prefix = StringUtils::replace(entry.first, ':', '_');
        const auto path   = m_store / (prefix + "_set.meta");
        FileUtils::create_if_not_existing(path);

        std::ofstream out_file(path, std::ios_base::app);
        out_file << entry.second << "\n";
    }
}

void FileKeyValueStoreClient::set_list(
    const std::vector<std::string>& keys,
    std::vector<std::vector<std::string>>& total_values) const
{
    for (const auto& key : keys) {
        const auto prefix = StringUtils::replace(key, ':', '_');
        const auto path   = m_store / (prefix + "_set.meta");

        std::vector<std::string> values;
        if (!std::filesystem::exists(path)) {
            total_values.push_back(values);
            continue;
        }

        File in_file(path);
        std::vector<std::string> file_values;
        in_file.read_lines(file_values);

        std::set<std::string> s(file_values.begin(), file_values.end());
        for (const auto& value : s) {
            values.push_back(value);
        }
        total_values.push_back(values);
    }
}

void FileKeyValueStoreClient::set_remove(const VecKeyValuePair& entries) const
{
    for (const auto& entry : entries) {
        const auto prefix = StringUtils::replace(entry.first, ':', '_');
        const auto path   = m_store / (prefix + "_set.meta");

        if (!std::filesystem::exists(path)) {
            return;
        }

        std::vector<std::string> file_values;
        {
            File in_file(path);
            in_file.read_lines(file_values);
        }

        std::set<std::string> s(file_values.begin(), file_values.end());
        s.erase(entry.second);

        std::vector<std::string> write_values;
        std::copy(s.begin(), s.end(), std::back_inserter(write_values));

        File out_file(path);
        out_file.write_lines(write_values);
    }
}

}  // namespace hestia