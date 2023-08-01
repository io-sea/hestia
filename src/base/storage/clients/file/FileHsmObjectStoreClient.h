#pragma once

#include "HsmObjectStoreClient.h"
#include "SerializeableWithFields.h"

#include <filesystem>

namespace hestia {
class FileHsmObjectStoreClientConfig : public SerializeableWithFields {
  public:
    FileHsmObjectStoreClientConfig() :
        SerializeableWithFields("file_hsm_object_store_client_config")
    {
        register_scalar_field(&m_root);
    }

    StringField m_root{"root", "object_store"};
};

class FileHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<FileHsmObjectStoreClient>;

    FileHsmObjectStoreClient();

    ~FileHsmObjectStoreClient();

    static Ptr create();

    static std::string get_registry_identifier();

    void initialize(
        const std::string& cache_path, const Dictionary& config) override;

    void do_initialize(
        const std::string& cache_path,
        const FileHsmObjectStoreClientConfig& config);

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