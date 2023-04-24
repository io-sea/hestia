#include "HestiaCppInterface.h"

#include "hestia_old.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include <ostk/InMemoryStreamSink.h>
#include <ostk/InMemoryStreamSource.h>

namespace rc {
int error(const ostk::RequestError<HsmServiceErrorCode>& error)
{
    switch (error.code()) {
        case HsmServiceErrorCode::NO_ERROR:
            return hestia::old::hestia_error_t::HESTIA_ERROR_OK;
        default:
            return hestia::old::hestia_error_t::HESTIA_ERROR_UNKOWN;
    }
}

int ok()
{
    return hestia::old::hestia_error_t::HESTIA_ERROR_OK;
}

int bad_stream()
{
    return hestia::old::hestia_error_t::HESTIA_ERROR_BAD_STREAM;
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
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::PUT);
    request.set_extent(extent);
    request.set_target_tier(tier_id);
    request.set_should_put_overwrite(overwrite);

    ostk::Stream stream;
    stream.setSource(ostk::InMemoryStreamSource::Create(buffer));

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(
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
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_extent(extent);
    request.set_source_tier(tier_id);

    ostk::Stream stream;
    stream.setSink(ostk::InMemoryStreamSink::Create(buffer));

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(
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
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::COPY);
    request.set_extent(extent);
    request.set_source_tier(source_tier);
    request.set_target_tier(target_tier);

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
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
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::MOVE);
    request.set_extent(extent);
    request.set_source_tier(source_tier);
    request.set_target_tier(target_tier);

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
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
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_query(keys);

    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }

    attributes = response->query_result();
    return rc::ok();
}

int HestiaCppInterface::set_attributes(
    const ostk::Uuid& object_id, const std::string& attributes)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_query(attributes);

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(const ostk::Uuid& object_id)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::REMOVE_ALL);
    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
        !response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::release(
    const ostk::Uuid& object_id, uint8_t tier_id, bool remove_key)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::REMOVE);
    request.set_source_tier(tier_id);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    return rc::ok();
}

int HestiaCppInterface::list_objects(
    uint8_t tier, std::vector<ostk::Uuid> objects)
{
    HsmServiceRequest request(HsmServiceRequestMethod::GET);
    request.set_source_tier(tier);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    objects = response->objects();

    return rc::ok();
}

int HestiaCppInterface::list_tiers(
    const ostk::Uuid& object_id, std::vector<uint8_t>& tiers)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET_TIERS);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    tiers = response->tiers();
    return rc::ok();
}

int HestiaCppInterface::list_attributes(
    const ostk::Uuid& object_id, std::string& attributes)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->getError());
    }
    attributes = response->query_result();
    return rc::ok();
}
}  // namespace hestia