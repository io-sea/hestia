#include "HsmStoreInterface.h"

#include "KeyValueStore.h"
#include "MultiBackendHsmObjectStoreClient.h"

HsmStoreInterface::HsmStoreInterface(
    std::unique_ptr<KeyValueStore> kvStore,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore) :
    mKeyValueStore(std::move(kvStore)), mObjectStore(std::move(objectStore))
{
}

HsmStoreInterface::Ptr HsmStoreInterface::Create(
    std::unique_ptr<KeyValueStore> kvStore,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore)
{
    return std::make_unique<HsmStoreInterface>(
        std::move(kvStore), std::move(objectStore));
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::copyData(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t sourceTier,
    uint8_t targetTier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::COPY);
    request.setExtent(request.extent());
    request.setSourceTier(request.sourceTier());
    request.setTargetTier(request.targetTier());
    return m_object_store->makeRequest(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::moveData(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t sourceTier,
    uint8_t targetTier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::MOVE);
    request.setExtent(request.extent());
    request.setSourceTier(request.sourceTier());
    request.setTargetTier(request.targetTier());
    return m_object_store->makeRequest(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::exists(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::EXISTS);
    return m_key_value_store->makeRequest(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::getMetadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::GET);
    return m_key_value_store->makeRequest(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::getData(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t sourceTier,
    ostk::Stream* stream)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::GET);
    request.setSourceTier(sourceTier);
    request.setExtent(extent);
    return m_object_store->makeRequest(request, stream);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::putData(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t sourceTier,
    uint8_t targetTier,
    bool overwrite,
    ostk::Stream* stream)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::PUT);
    request.setTargetTier(targetTier);
    if (overwrite) {
        request.object().updateModifiedTime();
    }
    else {
        request.object().initializeTimestamps();
    }
    return m_object_store->makeRequest(request, stream);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::putMetadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::PUT);
    return m_key_value_store->makeRequest(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::releaseData(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t sourceTier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::REMOVE);
    request.setSourceTier(sourceTier);
    request.setExtent(extent);
    return m_object_store->makeRequest(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::releaseMetadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->makeRequest(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::releaseMetadata(
    const ostk::StorageObject& object, uint8_t sourceTier)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->makeRequest(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::releaseData(
    const ostk::StorageObject& object, const ostk::Extent& extent)
{
    HsmObjectStoreRequest request(
        object, HsmObjectStoreRequestMethod::REMOVE_ALL);
    request.setExtent(extent);
    return m_object_store->makeRequest(request);
}
