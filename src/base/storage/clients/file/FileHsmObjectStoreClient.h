#pragma once

#include "FileObjectStoreClient.h"
#include "HsmObjectStoreClient.h"
#include "SerializeableWithFields.h"

#include <filesystem>

namespace hestia {

class FileHsmObjectStoreClientConfig : public SerializeableWithFields {
  public:
    FileHsmObjectStoreClientConfig();

    FileHsmObjectStoreClientConfig(const FileHsmObjectStoreClientConfig& other);

    FileHsmObjectStoreClientConfig& operator=(
        const FileHsmObjectStoreClientConfig& other);

    const std::string& get_root() const;

    FileObjectStoreClientConfig::Mode get_mode() const;

  private:
    void init();

    EnumField<
        FileObjectStoreClientConfig::Mode,
        FileObjectStoreClientConfig::Mode_enum_string_converter>
        m_mode{"mode", FileObjectStoreClientConfig::Mode::DATA_ONLY};
    StringField m_root{"root", "hsm_object_store"};
};

class FileHsmObjectStoreClient : public HsmObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<FileHsmObjectStoreClient>;

    FileHsmObjectStoreClient();

    ~FileHsmObjectStoreClient();

    static Ptr create();

    static std::string get_registry_identifier();

    void initialize(
        const std::string& id,
        const std::string& cache_path,
        const Dictionary& config) override;

    void do_initialize(
        const std::string& id,
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

    FileObjectStoreClient* get_client(uint8_t tier) const;

    std::filesystem::path m_store{"hsm_object_store"};

    FileObjectStoreClient::Ptr m_metadata_client;
    std::unordered_map<uint8_t, FileObjectStoreClient::Ptr> m_tier_clients;
};
}  // namespace hestia