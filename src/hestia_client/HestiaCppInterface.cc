#include "HestiaCppInterface.h"

#include "hestia.h"

#include "ApplicationContext.h"
#include "HsmService.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

namespace rc {
int error(const hestia::RequestError<hestia::HsmServiceErrorCode>& error)
{
    switch (error.code()) {
        case hestia::HsmServiceErrorCode::NO_ERROR:
            return hestia::hestia_error_t::HESTIA_ERROR_OK;
        case hestia::HsmServiceErrorCode::ERROR:
        case hestia::HsmServiceErrorCode::STL_EXCEPTION:
        case hestia::HsmServiceErrorCode::UNKNOWN_EXCEPTION:
        case hestia::HsmServiceErrorCode::UNSUPPORTED_REQUEST_METHOD:
        case hestia::HsmServiceErrorCode::OBJECT_NOT_FOUND:
        case hestia::HsmServiceErrorCode::BAD_PUT_OVERWRITE_COMBINATION:
        case hestia::HsmServiceErrorCode::MAX_ERROR:
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
    const hestia::Uuid& object_id,
    const hestia::Extent& extent,
    const hestia::ReadableBufferView& buffer,
    uint8_t tier_id,
    bool overwrite)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::PUT);
    request.set_extent(extent);
    request.set_target_tier(tier_id);
    request.set_should_put_overwrite(overwrite);

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(buffer));

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(
                request, &stream);
        !response->ok()) {
        return rc::error(response->get_error());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok()) {
        return rc::bad_stream();
    }
    return rc::ok();
}

int HestiaCppInterface::get(
    const hestia::Uuid& object_id,
    const hestia::Extent& extent,
    hestia::WriteableBufferView& buffer,
    uint8_t tier_id)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_extent(extent);
    request.set_source_tier(tier_id);

    hestia::Stream stream;
    stream.set_sink(hestia::InMemoryStreamSink::create(buffer));

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(
                request, &stream);
        !response->ok()) {
        return rc::error(response->get_error());
    }

    if (const auto stream_state = stream.flush(); !stream_state.ok()) {
        return rc::bad_stream();
    }
    return rc::ok();
}

int HestiaCppInterface::copy(
    const hestia::Uuid& object_id,
    const hestia::Extent& extent,
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
        return rc::error(response->get_error());
    }
    return rc::ok();
}

int HestiaCppInterface::move(
    const hestia::Uuid& object_id,
    const hestia::Extent& extent,
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
        return rc::error(response->get_error());
    }
    return rc::ok();
}

int HestiaCppInterface::get_attributes(
    const hestia::Uuid& object_id,
    const std::string& keys,
    std::string& attributes)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_query(keys);

    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->get_error());
    }

    attributes = response->query_result();
    return rc::ok();
}

int HestiaCppInterface::set_attributes(
    const hestia::Uuid& object_id, const std::string& attributes)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    request.set_query(attributes);

    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
        !response->ok()) {
        return rc::error(response->get_error());
    }
    return rc::ok();
}

int HestiaCppInterface::release(const hestia::Uuid& object_id)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::REMOVE_ALL);
    if (const auto response =
            ApplicationContext::get().get_hsm_service()->make_request(request);
        !response->ok()) {
        return rc::error(response->get_error());
    }
    return rc::ok();
}

int HestiaCppInterface::release(
    const hestia::Uuid& object_id, uint8_t tier_id, bool remove_key)
{
    (void)remove_key;

    HsmServiceRequest request(object_id, HsmServiceRequestMethod::REMOVE);
    request.set_source_tier(tier_id);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->get_error());
    }
    return rc::ok();
}

int HestiaCppInterface::list_objects(
    uint8_t tier, std::vector<hestia::Uuid> objects)
{
    HsmServiceRequest request(HsmServiceRequestMethod::GET);
    request.set_source_tier(tier);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->get_error());
    }
    objects = response->objects();

    return rc::ok();
}

int HestiaCppInterface::list_tiers(
    const hestia::Uuid& object_id, std::vector<uint8_t>& tiers)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET_TIERS);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->get_error());
    }
    tiers = response->tiers();
    return rc::ok();
}

int HestiaCppInterface::list_attributes(
    const hestia::Uuid& object_id, std::string& attributes)
{
    HsmServiceRequest request(object_id, HsmServiceRequestMethod::GET);
    const auto response =
        ApplicationContext::get().get_hsm_service()->make_request(request);
    if (!response->ok()) {
        return rc::error(response->get_error());
    }
    attributes = response->query_result();
    return rc::ok();
}
}  // namespace hestia