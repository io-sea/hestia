#pragma once

#include "KeyValueStoreClient.h"

#include <filesystem>

namespace hestia {
class FileKeyValueStoreClient : public KeyValueStoreClient {
  public:
    FileKeyValueStoreClient();

    ~FileKeyValueStoreClient() = default;

    void initialize(const Metadata& config) override;

  private:
    bool exists(const StorageObject& obj) const override;

    void get(StorageObject& obj, const std::vector<std::string>& keys = {})
        const override;

    void put(
        const StorageObject& obj,
        const std::vector<std::string>& keys = {}) const override;

    void remove(const StorageObject& obj) const override;

    void list(const Metadata::Query& query, std::vector<StorageObject>& fetched)
        const override;

    std::filesystem::path get_filename(const StorageObject& obj) const;

    std::filesystem::path m_store{"kv_store"};
};
}  // namespace hestia