#pragma once

#include "KeyValueStoreClient.h"
#include "SerializeableWithFields.h"

#include <filesystem>
#include <mutex>

namespace hestia {

class FileKeyValueStoreClientConfig : public SerializeableWithFields {
  public:
    FileKeyValueStoreClientConfig() :
        SerializeableWithFields("file_kv_store_config")
    {
        register_scalar_field(&m_root);
    }
    StringField m_root{"root", "kv_store"};
};

class FileKeyValueStoreClient : public KeyValueStoreClient {
  public:
    FileKeyValueStoreClient();

    virtual ~FileKeyValueStoreClient();

    void initialize(
        const std::string& cache_path, const Dictionary& config) override;

    void do_initialize(
        const std::string& cache_path,
        const FileKeyValueStoreClientConfig& config);

  private:
    void string_exists(
        const std::vector<std::string>& key,
        std::vector<bool>& found) const override;

    void string_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const override;

    void string_set(const VecKeyValuePair& kv_pairs) const override;

    void string_remove(const std::vector<std::string>& key) const override;

    void set_add(const VecKeyValuePair& entry) const override;

    void set_list(
        const std::vector<std::string>& key,
        std::vector<std::vector<std::string>>& values) const override;

    void set_remove(const VecKeyValuePair& entry) const override;

    std::filesystem::path m_store{"kv_store"};
    std::string m_db_name{"strings_db.json"};
};
}  // namespace hestia