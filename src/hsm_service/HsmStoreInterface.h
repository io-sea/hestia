#pragma once

#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"

#include "Stream.h"

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class HsmKeyValueStore;

class HsmStoreInterface {
  public:
    using Ptr = std::unique_ptr<HsmStoreInterface>;

    HsmStoreInterface(
        std::unique_ptr<HsmKeyValueStore> kv_store,
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store);

    static Ptr create(
        std::unique_ptr<HsmKeyValueStore> kv_store,
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store);

    ObjectStoreResponse::Ptr exists(const StorageObject& object);

    ObjectStoreResponse::Ptr get_metadata(const StorageObject& object);

    HsmObjectStoreResponse::Ptr get_data(
        const StorageObject& object,
        const Extent& extent,
        uint8_t source_tier,
        Stream* stream);

    ObjectStoreResponse::Ptr put_metadata(const StorageObject& object);

    HsmObjectStoreResponse::Ptr put_data(
        const StorageObject& object,
        const Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier,
        bool overwrite,
        Stream* stream);

    HsmObjectStoreResponse::Ptr copy_data(
        const StorageObject& object,
        const Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    HsmObjectStoreResponse::Ptr move_data(
        const StorageObject& object,
        const Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    ObjectStoreResponse::Ptr release_metadata(const StorageObject& object);

    ObjectStoreResponse::Ptr release_metadata(
        const StorageObject& object, uint8_t source_tier);

    HsmObjectStoreResponse::Ptr release_data(
        const StorageObject& object, const Extent& extent, uint8_t source_tier);

    HsmObjectStoreResponse::Ptr release_data(
        const StorageObject& object, const Extent& extent);


  private:
    std::unique_ptr<HsmKeyValueStore> m_key_value_store;
    std::unique_ptr<MultiBackendHsmObjectStoreClient> m_object_store;
};
}  // namespace hestia