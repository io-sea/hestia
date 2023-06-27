#pragma once

#include "KeyValueStoreClient.h"

#include <filesystem>

namespace hestia {

struct FileKeyValueStoreClientConfig {
    std::filesystem::path m_root{"kv_store"};
};

class FileKeyValueStoreClient : public KeyValueStoreClient {
  public:
    FileKeyValueStoreClient();

    virtual ~FileKeyValueStoreClient() = default;

    void initialize(const Metadata& config) override;

    void do_initialize(const FileKeyValueStoreClientConfig& config);

  private:
    bool string_exists(const std::string& key) const override;

    void string_get(const std::string& key, std::string& value) const override;

    void string_multi_get(
        const std::vector<std::string>& key,
        std::vector<std::string>& value) const override;

    void string_set(
        const std::string& key, const std::string& value) const override;

    void string_remove(const std::string& key) const override;

    void set_add(const std::string& key, const Uuid& value) const override;

    void set_list(
        const std::string& key, std::vector<Uuid>& value) const override;

    void set_remove(const std::string& key, const Uuid& value) const override;

    std::filesystem::path m_store{"kv_store"};
    std::string m_db_name{"strings_db.json"};
};
}  // namespace hestia