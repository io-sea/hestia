#include "HsmStoreInterface.h"

#include "KeyValueStore.h"
#include "MultiBackendHsmObjectStoreClient.h"

namespace hestia {
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
    const hestia::StorageObject& object,
    const hestia::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    (void)extent;
    (void)source_tier;
    (void)target_tier;

    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::COPY);
    request.set_extent(request.extent());
    request.set_source_tier(request.source_tier());
    request.set_target_tier(request.target_tier());
    return m_object_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::move_data(
    const hestia::StorageObject& object,
    const hestia::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    (void)extent;
    (void)source_tier;
    (void)target_tier;

    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::MOVE);
    request.set_extent(request.extent());
    request.set_source_tier(request.source_tier());
    request.set_target_tier(request.target_tier());
    return m_object_store->make_request(request);
}

hestia::ObjectStoreResponse::Ptr HsmStoreInterface::exists(
    const hestia::StorageObject& object)
{
    hestia::ObjectStoreRequest request(
        object, hestia::ObjectStoreRequestMethod::EXISTS);
    return m_key_value_store->make_request(request);
}

hestia::ObjectStoreResponse::Ptr HsmStoreInterface::get_metadata(
    const hestia::StorageObject& object)
{
    hestia::ObjectStoreRequest request(
        object, hestia::ObjectStoreRequestMethod::GET);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::get_data(
    const hestia::StorageObject& object,
    const hestia::Extent& extent,
    uint8_t source_tier,
    hestia::Stream* stream)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::GET);
    request.set_source_tier(source_tier);
    request.set_extent(extent);
    return m_object_store->make_request(request, stream);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::put_data(
    const hestia::StorageObject& object,
    const hestia::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier,
    bool overwrite,
    hestia::Stream* stream)
{
    (void)extent;
    (void)source_tier;
    (void)target_tier;

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

hestia::ObjectStoreResponse::Ptr HsmStoreInterface::put_metadata(
    const hestia::StorageObject& object)
{
    hestia::ObjectStoreRequest request(
        object, hestia::ObjectStoreRequestMethod::PUT);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::release_data(
    const hestia::StorageObject& object,
    const hestia::Extent& extent,
    uint8_t source_tier)
{
    HsmObjectStoreRequest request(object, HsmObjectStoreRequestMethod::REMOVE);
    request.set_source_tier(source_tier);
    request.set_extent(extent);
    return m_object_store->make_request(request);
}

hestia::ObjectStoreResponse::Ptr HsmStoreInterface::release_metadata(
    const hestia::StorageObject& object)
{
    hestia::ObjectStoreRequest request(
        object, hestia::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->make_request(request);
}

hestia::ObjectStoreResponse::Ptr HsmStoreInterface::release_metadata(
    const hestia::StorageObject& object, uint8_t source_tier)
{
    (void)source_tier;
    hestia::ObjectStoreRequest request(
        object, hestia::ObjectStoreRequestMethod::REMOVE);
    return m_key_value_store->make_request(request);
}

HsmObjectStoreResponse::Ptr HsmStoreInterface::release_data(
    const hestia::StorageObject& object, const hestia::Extent& extent)
{
    HsmObjectStoreRequest request(
        object, HsmObjectStoreRequestMethod::REMOVE_ALL);
    request.set_extent(extent);
    return m_object_store->make_request(request);
}
}  // namespace hestia
