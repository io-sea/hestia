#pragma once

#include "HsmObjectStoreClient.h"

#include <filesystem>

namespace hestia {
struct FileHsmObjectStoreClientConfig {
    std::filesystem::path m_store_location{"object_store"};
};

class FileHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<FileHsmObjectStoreClient>;

    FileHsmObjectStoreClient();

    ~FileHsmObjectStoreClient();

    static Ptr create();

    static std::string get_registry_identifier();

    void do_initialize(const FileHsmObjectStoreClientConfig& config);

  private:
    void put(
        const HsmObjectStoreRequest& request, Stream* stream) const override;

    void get(
        const HsmObjectStoreRequest& request,
        StorageObject& object,
        Stream* stream) const override;

    void remove(const HsmObjectStoreRequest& request) const override;

    void copy(const HsmObjectStoreRequest& request) const override;

    void move(const HsmObjectStoreRequest& request) const override;

    std::filesystem::path get_tier_path(uint8_t tier) const;
    std::filesystem::path m_store{"hsm_object_store"};
};
}  // namespace hestia