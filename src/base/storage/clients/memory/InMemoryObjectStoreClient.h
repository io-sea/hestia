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

    bool exists(const Uuid& object_id) const;

    void list(
        const KeyValuePair& query,
        std::vector<StorageObject>& matching_objects) const override;

    mutable BlockStore m_data;
    mutable std::unordered_map<std::string, Map> m_metadata;
};
}  // namespace hestia