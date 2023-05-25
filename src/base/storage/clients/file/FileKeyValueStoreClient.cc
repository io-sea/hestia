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

void FileKeyValueStoreClient::initialize(const Metadata& config_data)
{
    FileKeyValueStoreClientConfig config;
    auto on_item = [&config](const std::string& key, const std::string& value) {
        if (key == "root") {
            config.m_root = value;
        }
    };
    config_data.for_each_item(on_item);
    do_initialize(config);
}

void FileKeyValueStoreClient::do_initialize(
    const FileKeyValueStoreClientConfig& config)
{
    m_store = config.m_root;
    LOG_INFO("Initializing at: " + m_store.string());
}

void FileKeyValueStoreClient::string_get(
    const std::string& key, std::string& value) const
{
    JsonUtils::get_value(m_store / m_db_name, key, value);
}

void FileKeyValueStoreClient::string_multi_get(
    const std::vector<std::string>& keys,
    std::vector<std::string>& values) const
{
    JsonUtils::get_values(m_store / m_db_name, keys, values);
}

void FileKeyValueStoreClient::string_set(
    const std::string& key, const std::string& value) const
{
    const auto path = m_store / m_db_name;
    FileUtils::create_if_not_existing(path);
    LOG_INFO("Setting: " + key + " to " + value + " at " + path.string());
    JsonUtils::set_value(path, key, value);
}

void FileKeyValueStoreClient::string_remove(const std::string& key) const
{
    JsonUtils::remove_key(m_store / m_db_name, key);
}

bool FileKeyValueStoreClient::string_exists(const std::string& key) const
{
    return JsonUtils::has_key(m_store / m_db_name, key);
}

void FileKeyValueStoreClient::set_add(
    const std::string& key, const std::string& value) const
{
    LOG_INFO("Adding to set: " + key + " " + value);

    const auto prefix = StringUtils::replace(key, ':', '_');
    const auto path   = m_store / (prefix + "_set.meta");
    FileUtils::create_if_not_existing(path);

    std::ofstream out_file(path, std::ios_base::app);
    out_file << value << "\n";
}

void FileKeyValueStoreClient::set_list(
    const std::string& key, std::vector<std::string>& values) const
{
    LOG_INFO("FileKeyValueStoreClient SET LIST");
    const auto prefix = StringUtils::replace(key, ':', '_');
    const auto path   = m_store / (prefix + "_set.meta");

    if (!std::filesystem::exists(path)) {
        return;
    }

    File in_file(path);
    std::vector<std::string> file_values;
    in_file.read_lines(file_values);

    std::set<std::string> s(file_values.begin(), file_values.end());
    std::copy(s.begin(), s.end(), std::back_inserter(values));
}

void FileKeyValueStoreClient::set_remove(
    const std::string& key, const std::string& value) const
{
    const auto prefix = StringUtils::replace(key, ':', '_');
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
    s.erase(value);

    std::vector<std::string> write_values;
    std::copy(s.begin(), s.end(), std::back_inserter(write_values));

    File out_file(path);
    out_file.write_lines(write_values);
}

}  // namespace hestia