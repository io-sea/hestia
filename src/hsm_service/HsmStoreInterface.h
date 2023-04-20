#pragma once

#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"

#include <ostk/Stream.h>

class MultiBackendHsmObjectStoreClient;
class KeyValueStore;

class HsmStoreInterface
{
public:
    using Ptr = std::unique_ptr<HsmStoreInterface>;

    HsmStoreInterface(std::unique_ptr<KeyValueStore> kvStore, 
        std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore);

    static Ptr Create(std::unique_ptr<KeyValueStore> kvStore, 
        std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore);

    ostk::ObjectStoreResponse::Ptr exists(const ostk::StorageObject& object);

    ostk::ObjectStoreResponse::Ptr getMetadata(const ostk::StorageObject& object);
    HsmObjectStoreResponse::Ptr getData(const ostk::StorageObject& object, const ostk::Extent& extent, uint8_t sourceTier, ostk::Stream* stream);

    ostk::ObjectStoreResponse::Ptr putMetadata(const ostk::StorageObject& object);
    HsmObjectStoreResponse::Ptr putData(const ostk::StorageObject& object, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier, bool overwrite, ostk::Stream* stream);

    HsmObjectStoreResponse::Ptr copyData(const ostk::StorageObject& object, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier);
    HsmObjectStoreResponse::Ptr moveData(const ostk::StorageObject& object, const ostk::Extent& extent, uint8_t sourceTier, uint8_t targetTier);

    ostk::ObjectStoreResponse::Ptr releaseMetadata(const ostk::StorageObject& object);
    ostk::ObjectStoreResponse::Ptr releaseMetadata(const ostk::StorageObject& object, uint8_t sourceTier);
    HsmObjectStoreResponse::Ptr releaseData(const ostk::StorageObject& object, const ostk::Extent& extent, uint8_t sourceTier);
    HsmObjectStoreResponse::Ptr releaseData(const ostk::StorageObject& object, const ostk::Extent& extent);


private:
    std::unique_ptr<KeyValueStore> mKeyValueStore;
    std::unique_ptr<MultiBackendHsmObjectStoreClient> mObjectStore;
};