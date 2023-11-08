#pragma once

#include "ObjectStoreClient.h"

#include "EnumUtils.h"

#include <filesystem>

namespace hestia {

class FileObjectStoreClientConfig : public SerializeableWithFields {
  public:
    STRINGABLE_ENUM(Mode, DATA_AND_METADATA, DATA_ONLY, METADATA_ONLY)

    FileObjectStoreClientConfig() :
        SerializeableWithFields("file_object_store_client_config")
    {
        register_scalar_field(&m_root);
        register_scalar_field(&m_mode);
    }

    void set_mode(Mode mode) { m_mode.init_value(mode); }

    EnumField<Mode, Mode_enum_string_converter> m_mode{"mode", Mode::DATA_ONLY};
    StringField m_root{"root", "object_store"};
};

class FileObjectStoreClient : public ObjectStoreClient {
  public:
    enum class Mode { DATA_AND_METADATA, DATA_ONLY, METADATA_ONLY };

    using Ptr = std::unique_ptr<FileObjectStoreClient>;

    virtual ~FileObjectStoreClient() = default;

    static Ptr create();

    static std::string get_registry_identifier();

    void initialize(
        const std::string& id,
        const std::string& cache_path,
        const Dictionary& config) override;

    void do_initialize(
        const std::string& id,
        const std::string& cache_path,
        const FileObjectStoreClientConfig& config);

    void migrate(
        const std::string& object_id,
        const std::filesystem::path& root,
        bool keep_existing = true);

  private:
    // ObjectStoreClient API
    bool exists(const StorageObject& object) const override;

    void get(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void put(
        const ObjectStoreRequest& request,
        completionFunc completion_func,
        Stream* stream                     = nullptr,
        Stream::progressFunc progress_func = nullptr) const override;

    void remove(const StorageObject& object) const override;

    bool exists(const std::string& object_key) const;

    void list(
        const KeyValuePair& query,
        std::vector<StorageObject>& matching_objects) const override;

    // Internal API
    std::filesystem::path get_data_path(
        const std::string& object_key,
        const std::filesystem::path& root = {}) const;
    std::filesystem::path get_metadata_path(
        const std::string& object_key,
        const std::filesystem::path& root = {}) const;

    bool needs_metadata() const;

    bool needs_data() const;

    std::string get_metadata_item(
        const std::string& object_key, const std::string& key) const;

    bool has_data(const std::string& object_key) const;

    void read_metadata(StorageObject& object) const;

    FileObjectStoreClientConfig m_config;
    std::filesystem::path m_root{"object_store"};
    FileObjectStoreClientConfig::Mode m_mode{
        FileObjectStoreClientConfig::Mode::DATA_ONLY};
};
}  // namespace hestia