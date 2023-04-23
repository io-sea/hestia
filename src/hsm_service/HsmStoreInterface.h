#pragma once

#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"

#include <ostk/Stream.h>

class MultiBackendHsmObjectStoreClient;
class KeyValueStore;

class HsmStoreInterface {
  public:
    using Ptr = std::unique_ptr<HsmStoreInterface>;

    HsmStoreInterface(
        std::unique_ptr<KeyValueStore> kv_store,
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store);

    static Ptr create(
        std::unique_ptr<KeyValueStore> kv_store,
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store);

    ostk::ObjectStoreResponse::Ptr exists(const ostk::StorageObject& object);

    ostk::ObjectStoreResponse::Ptr get_metadata(
        const ostk::StorageObject& object);
    HsmObjectStoreResponse::Ptr get_data(
        const ostk::StorageObject& object,
        const ostk::Extent& extent,
        uint8_t source_tier,
        ostk::Stream* stream);

    ostk::ObjectStoreResponse::Ptr put_metadata(
        const ostk::StorageObject& object);
    HsmObjectStoreResponse::Ptr put_data(
        const ostk::StorageObject& object,
        const ostk::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier,
        bool overwrite,
        ostk::Stream* stream);

    HsmObjectStoreResponse::Ptr copy_data(
        const ostk::StorageObject& object,
        const ostk::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);
    HsmObjectStoreResponse::Ptr move_data(
        const ostk::StorageObject& object,
        const ostk::Extent& extent,
        uint8_t source_tier,
        uint8_t target_tier);

    ostk::ObjectStoreResponse::Ptr release_metadata(
        const ostk::StorageObject& object);
    ostk::ObjectStoreResponse::Ptr release_metadata(
        const ostk::StorageObject& object, uint8_t source_tier);
    HsmObjectStoreResponse::Ptr release_data(
        const ostk::StorageObject& object,
        const ostk::Extent& extent,
        uint8_t source_tier);
    HsmObjectStoreResponse::Ptr release_data(
        const ostk::StorageObject& object, const ostk::Extent& extent);


  private:
    std::unique_ptr<KeyValueStore> m_key_value_store;
    std::unique_ptr<MultiBackendHsmObjectStoreClient> m_object_store;
};