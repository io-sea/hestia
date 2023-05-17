#pragma once

#include "ObjectStoreResponse.h"
#include "StorageTier.h"

namespace hestia {
class KeyValueStoreClient;

class HsmKeyValueStore {
  public:
    HsmKeyValueStore(std::unique_ptr<KeyValueStoreClient> client);

    virtual ~HsmKeyValueStore();

    [[nodiscard]] ObjectStoreResponse::Ptr make_request(
        const ObjectStoreRequest& request) const noexcept;

  private:
    void get(StorageObject& object) const;

    void put(const StorageObject& object) const;

    bool exists(const StorageObject& object) const;

    void remove(const StorageObject& object) const;

    void get(StorageTier& tier) const;

    void put(const StorageTier& tier) const;

    void list_objects(std::vector<StorageObject>& objects) const;

    void list_tiers(std::vector<StorageTier>& tiers) const;

  private:
    void on_exception(
        const ObjectStoreRequest& request,
        ObjectStoreResponse* response,
        const std::string& message = {}) const;

    void on_exception(
        const ObjectStoreRequest& request,
        ObjectStoreResponse* response,
        const ObjectStoreError& error) const;

    std::unique_ptr<KeyValueStoreClient> m_client;
};
}  // namespace hestia
