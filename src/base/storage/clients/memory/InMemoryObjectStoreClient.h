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

    void migrate(
        const std::string& object_id,
        InMemoryObjectStoreClient* target_client,
        bool delete_after = false);

    std::string dump() const;

  private:
    bool exists(const StorageObject& object) const override;

    void get(ObjectStoreContext& ctx) const override;

    void put(ObjectStoreContext& ctx) const override;

    void remove(const StorageObject& object) const override;

    void list(
        const KeyValuePair& query,
        std::vector<StorageObject>& matching_objects) const override;

    void upsert_metadata(const StorageObject& object) const;

    void read_metadata(StorageObject& object) const;

    mutable BlockStore m_data;
    mutable std::unordered_map<std::string, Map> m_metadata;

    mutable std::mutex m_data_mutex;
    mutable std::mutex m_metadata_mutex;
};
}  // namespace hestia