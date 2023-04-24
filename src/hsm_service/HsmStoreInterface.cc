#include "HsmStoreInterface.h"

#include "KeyValueStore.h"
#include "MultiBackendHsmObjectStoreClient.h"

HsmStoreInterface::HsmStoreInterface(
    std::unique_ptr<KeyValueStore> kv_store,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store) :
    m_key_value_store(std::move(kv_store)),
    m_object_store(std::move(object_store))
{
}

HsmStoreInterface::Ptr HsmStoreInterface::create(
    std::unique_ptr<KeyValueStore> kv_store,
    std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store)
{
    return std::make_unique<HsmStoreInterface>(
        std::move(kv_store), std::move(object_store));
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::copy_data(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::COPY);
    request.set_extent(request.extent());
    request.set_source_tier(request.source_tier());
    request.set_target_tier(request.target_tier());
    return m_object_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::move_data(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::MOVE);
    request.set_extent(request.extent());
    request.set_source_tier(request.source_tier());
    request.set_target_tier(request.target_tier());
    return m_object_store->make_request(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::exists(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::EXISTS);
    return m_key_value_store->make_request(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::get_metadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::GET);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::get_data(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t source_tier,
    ostk::Stream* stream)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::GET);
    request.set_source_tier(source_tier);
    request.set_extent(extent);
    return m_object_store->make_request(request, stream);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::put_data(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier,
    bool overwrite,
    ostk::Stream* stream)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::PUT);
    request.set_target_tier(target_tier);
    if (overwrite) {
        request.object().update_modified_time();
    }
    else {
        request.object().initialize_timestamps();
    }
    return m_object_store->make_request(request, stream);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::put_metadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::PUT);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::release_data(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    uint8_t source_tier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::REMOVE);
    request.set_source_tier(source_tier);
    request.set_extent(extent);
    return m_object_store->make_request(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::release_metadata(
    const ostk::StorageObject& object)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->make_request(request);
}

ostk::ObjectStoreResponse::Ptr HsmStoreInterface::release_metadata(
    const ostk::StorageObject& object, uint8_t source_tier)
{
    ostk::ObjectStoreRequest request(
        object, ostk::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::release_data(
    const ostk::StorageObject& object, const ostk::Extent& extent)
{
    HsmObjectStoreRequest request(
        object, HsmObjectStoreRequestMethod::REMOVE_ALL);
    request.set_extent(extent);
    return m_object_store->make_request(request);
}
