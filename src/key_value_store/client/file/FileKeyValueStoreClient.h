#pragma once

#include "KeyValueStoreClient.h"

#include <filesystem>

class FileKeyValueStoreClient : public KeyValueStoreClient {
  public:
    FileKeyValueStoreClient();

    ~FileKeyValueStoreClient() = default;

  private:
    bool exists(const ostk::StorageObject& obj) const override;

    void get(
        ostk::StorageObject& obj,
        const std::vector<std::string>& keys = {}) const override;

    void put(
        const ostk::StorageObject& obj,
        const std::vector<std::string>& keys = {}) const override;

    void remove(const ostk::StorageObject& obj) const override;

    void list(
        const ostk::Metadata::Query& query,
        std::vector<ostk::StorageObject>& fetched) const override;

    std::filesystem::path getFilename(const ostk::StorageObject& obj) const;

    // const char mDelimiter = ';';
    const std::filesystem::directory_entry mStore{"kv_store"};
};