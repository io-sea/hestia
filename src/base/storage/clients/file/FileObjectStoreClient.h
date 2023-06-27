#pragma once

#include "ObjectStoreClient.h"

#include <filesystem>

namespace hestia {
class FileObjectStoreClient : public ObjectStoreClient {
  public:
    enum class Mode { DATA_AND_METADATA, DATA_ONLY, METADATA_ONLY };

    using Ptr = std::unique_ptr<FileObjectStoreClient>;

    virtual ~FileObjectStoreClient() = default;

    static Ptr create();

    static Ptr create(
        const std::filesystem::path& root, Mode mode = Mode::DATA_AND_METADATA);

    static std::string get_registry_identifier();

    void do_initialize(
        const std::filesystem::path& root, Mode mode = Mode::DATA_AND_METADATA);

    void migrate(
        const Uuid& object_id,
        const std::filesystem::path& root,
        bool keep_existing = true);

  private:
    // ObjectStoreClient API
    bool exists(const StorageObject& object) const override;

    void get(StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void put(const StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void remove(const StorageObject& object) const override;

    bool exists(const Uuid& object_id) const;

    void list(
        const Metadata::Query& query,
        std::vector<StorageObject>& matching_objects) const override;

    // Internal API
    std::filesystem::path get_data_path(
        const Uuid& object_id, const std::filesystem::path& root = {}) const;
    std::filesystem::path get_metadata_path(
        const Uuid& object_id, const std::filesystem::path& root = {}) const;

    std::string get_metadata_item(
        const Uuid& object_id, const std::string& key) const;

    bool has_data(const Uuid& object_id) const;

    void read_metadata(StorageObject& object) const;

    Mode m_mode{Mode::DATA_AND_METADATA};
    std::filesystem::path m_root;
};
}  // namespace hestia