#include "HestiaCppInterface.h"

#include "hestia.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include <ostk/InMemoryStreamSink.h>
#include <ostk/InMemoryStreamSource.h>

namespace rc {
int error(const ostk::RequestError<HsmServiceErrorCode>& error)
{
    switch (error.code()) {
        case HsmServiceErrorCode::NO_ERROR:
            return hestia::hestia_error_t::HESTIA_ERROR_OK;
        default:
            return hestia::hestia_error_t::HESTIA_ERROR_UNKOWN;
    }
}

int ok()
{
    return hestia::hestia_error_t::HESTIA_ERROR_OK;
}

int bad_stream()
{
    return hestia::hestia_error_t::HESTIA_ERROR_BAD_STREAM;
}

}  // namespace rc

namespace hestia {
int HestiaCppInterface::put(
    const ostk::Uuid& object_id,
    const ostk::Extent& extent,
    const ostk::ReadableBufferView& buffer,
    uint8_t tier_id,
    bool overwrite)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::PUT);
    request.setExtent(extent);
    request.setTargetTier(tier_id);
    request.setShouldPutOverwrite(overwrite);

    ostk::Stream stream;
    stream.setSource(ostk::InMemoryStreamSource::Create(buffer));

    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(
                request, &stream);
        !response->ok()) {
        return rc::error(response->getError());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok()) {
        return rc::bad_stream();
    }
    return rc::ok();
}

int HestiaCppInterface::get(
    const ostk::Uuid& object_id,
    const ostk::Extent& extent,
    ostk::WriteableBufferView& buffer,
    uint8_t tier_id)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setExtent(extent);
    request.setSourceTier(tier_id);

    ostk::Stream stream;
    stream.setSink(ostk::InMemoryStreamSink::Create(buffer));

    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(
                request, &stream);
        !response->ok()) {
        return rc::error(response->getError());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok()) {
        return rc::bad_stream();
    }
    return rc::ok();
}

int HestiaCppInterface::copy(
    const ostk::Uuid& object_id,
    const ostk::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::COPY);
    request.setExtent(extent);
    request.setSourceTier(source_tier);
    request.setTargetTier(target_tier);

    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::move(
    const ostk::Uuid& object_id,
    const ostk::Extent& extent,
    uint8_t source_tier,
    uint8_t target_tier)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::MOVE);
    request.setExtent(extent);
    request.setSourceTier(source_tier);
    request.setTargetTier(target_tier);

    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::get_attributes(
    const ostk::Uuid& object_id,
    const std::string& keys,
    std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setQuery(keys);

    const auto response =
        ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }

    attributes = response->queryResult();
    return rc::ok();
}

int HestiaCppInterface::set_attributes(
    const ostk::Uuid& object_id, const std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    request.setQuery(attributes);

    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(const ostk::Uuid& object_id)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::REMOVE_ALL);
    if (const auto response =
            ApplicationContext::get().getHsmService()->makeRequest(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(
    const ostk::Uuid& object_id, uint8_t tier_id, bool remove_key)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::REMOVE);
    request.setSourceTier(tier_id);
    const auto response =
        ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::list_objects(
    uint8_t tier, std::vector<ostk::Uuid> objects)
{
    HsmServiceRequest request(HsmServiceRequestMethod::GET);
    request.setSourceTier(tier);
    const auto response =
        ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    objects = response->objects();

    return rc::ok();
}

int HestiaCppInterface::list_tiers(
    const ostk::Uuid& object_id, std::vector<uint8_t>& tiers)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET_TIERS);
    const auto response =
        ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    tiers = response->tiers();
    return rc::ok();
}

int HestiaCppInterface::list_attributes(
    const ostk::Uuid& object_id, std::string& attributes)
{
    HsmServiceRequest request(objectId, HsmServiceRequestMethod::GET);
    const auto response =
        ApplicationContext::get().getHsmService()->makeRequest(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    attributes = response->queryResult();
    return rc::ok();
}
}  // namespace hestia