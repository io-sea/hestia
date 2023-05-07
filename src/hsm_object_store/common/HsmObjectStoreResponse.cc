#include "HsmObjectStoreResponse.h"

namespace hestia {
HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request,
    HsmMiddlewareResponse::Ptr middleware_response) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request),
    m_middleware_response(std::move(middleware_response))
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsm_child_response) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
    (void)hsm_child_response;
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request,
    ObjectStoreResponse::Ptr child_response) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
    (void)child_response;
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const BaseObjectStoreRequest& request)
{
    return std::make_unique<HsmObjectStoreResponse>(request);
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const BaseObjectStoreRequest& request,
    HsmMiddlewareResponse::Ptr middleware_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(middleware_response));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsm_child_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(hsm_child_response));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const BaseObjectStoreRequest& request,
    ObjectStoreResponse::Ptr child_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(child_response));
}

ObjectStoreResponse::Ptr HsmObjectStoreResponse::to_base_response(
    const BaseObjectStoreRequest& request,
    const HsmObjectStoreResponse* response)
{
    auto base_response = ObjectStoreResponse::create(request);
    if (!response->ok()) {
        base_response->on_error(
            {ObjectStoreErrorCode::ERROR, response->get_error().to_string()});
    }
    else {
        base_response->object() = response->m_object;
        base_response->set_object_found(response->object_found());
    }
    return base_response;
}
}  // namespace hestia