#include "FileKeyValueStoreClient.h"

#include "File.h"
#include "FileUtils.h"
#include "JsonUtils.h"

#include <fstream>
#include <iterator>
#include <set>

#include "Logger.h"

namespace hestia {
FileKeyValueStoreClient::FileKeyValueStoreClient() {}

void FileKeyValueStoreClient::initialize(const Metadata& config)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        if (key == "root") {
            m_store = value;
        }
    };
    config.for_each_item(on_item);
    LOG_INFO("Initializing at: " + m_store.string());
}

void FileKeyValueStoreClient::string_get(
    const std::string& key, std::string& value) const
{
    JsonUtils::get_value(m_store / "strings_db.meta", key, value);
}

void FileKeyValueStoreClient::string_set(
    const std::string& key, const std::string& value) const
{
    JsonUtils::set_value(m_store / "strings_db.meta", key, value);
}

void FileKeyValueStoreClient::string_remove(const std::string& key) const
{
    JsonUtils::remove_key(m_store / "strings_db.meta", key);
}

bool FileKeyValueStoreClient::string_exists(const std::string& key) const
{
    return JsonUtils::has_key(m_store / "strings_db.meta", key);
}

void FileKeyValueStoreClient::set_add(
    const std::string& key, const std::string& value) const
{
    const auto path = m_store / (key + "_set.meta");
    FileUtils::create_if_not_existing(path);

    std::ofstream out_file(path);
    out_file << value << "\n";
}

void FileKeyValueStoreClient::set_list(
    const std::string& key, std::vector<std::string>& values) const
{
    const auto path = m_store / (key + "_set.meta");

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
    const auto path = m_store / (key + "_set.meta");

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