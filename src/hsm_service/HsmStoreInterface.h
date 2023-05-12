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

    hestia::ObjectStoreResponse::Ptr exists(
        const hestia::StorageObject& object);

    hestia::ObjectStoreResponse::Ptr get_metadata(
        const hestia::StorageObject& object);
    HsmObjectStoreResponse::Ptr get_data(
        const hestia::StorageObject& object,
        const hestia::Extent& extent,
        uint8_t source_tier,
        hestia::Stream* stream);

    hestia::ObjectStoreResponse::Ptr put_metadata(
        const hestia::StorageObject& object);
    HsmObjectStoreResponse::Ptr put_data(
        const hestia::StorageObject& object,
        const hestia::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier,
        bool overwrite,
        hestia::Stream* stream);

    HsmObjectStoreResponse::Ptr copy_data(
        const hestia::StorageObject& object,
        const hestia::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);
    HsmObjectStoreResponse::Ptr move_data(
        const hestia::StorageObject& object,
        const hestia::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    hestia::ObjectStoreResponse::Ptr release_metadata(
        const hestia::StorageObject& object);
    hestia::ObjectStoreResponse::Ptr release_metadata(
        const hestia::StorageObject& object, uint8_t source_tier);
    HsmObjectStoreResponse::Ptr release_data(
        const hestia::StorageObject& object,
        const hestia::Extent& extent,
        uint8_t source_tier);
    HsmObjectStoreResponse::Ptr release_data(
        const hestia::StorageObject& object, const hestia::Extent& extent);


  private:
    std::unique_ptr<HsmKeyValueStore> m_key_value_store;
    std::unique_ptr<MultiBackendHsmObjectStoreClient> m_object_store;
};
}  // namespace hestia