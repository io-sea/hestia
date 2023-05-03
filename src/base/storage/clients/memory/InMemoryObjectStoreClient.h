#pragma once

#include "BlockStore.h"
#include "ObjectStoreClient.h"

namespace hestia {
class InMemoryObjectStoreClient : public ObjectStoreClient {
  public:
    using Ptr = std::unique_ptr<InMemoryObjectStoreClient>;

    virtual ~InMemoryObjectStoreClient() = default;

    static Ptr create();

    static std::string get_registry_identifier();

  private:
    bool exists(const StorageObject& object) const override;

    void get(StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void put(const StorageObject& object, const Extent& extent, Stream* stream)
        const override;

    void remove(const StorageObject& object) const override;

    bool exists(const std::string& object_id) const;

    void list(
        const Metadata::Query& query,
        std::vector<StorageObject>& matching_objects) const override;

    mutable BlockStore m_data;
    mutable std::unordered_map<std::string, hestia::Metadata> m_metadata;
};
}  // namespace hestia