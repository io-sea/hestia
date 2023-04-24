#include "HsmObjectStoreResponse.h"

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const ostk::BaseObjectStoreRequest& request) :
    ostk::BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const ostk::BaseObjectStoreRequest& request,
    HsmMiddlewareResponse::Ptr middleware_response) :
    ostk::BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request),
    m_middleware_response(std::move(middleware_response))
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const ostk::BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsm_child_response) :
    ostk::BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const ostk::BaseObjectStoreRequest& request,
    ostk::ObjectStoreResponse::Ptr child_response) :
    ostk::BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request)
{
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request)
{
    return std::make_unique<HsmObjectStoreResponse>(request);
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request,
    HsmMiddlewareResponse::Ptr middleware_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(middleware_response));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsm_child_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(hsm_child_response));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request,
    ostk::ObjectStoreResponse::Ptr child_response)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(child_response));
}

ostk::ObjectStoreResponse::Ptr HsmObjectStoreResponse::to_base_response(
    const ostk::BaseObjectStoreRequest& request,
    const HsmObjectStoreResponse* response)
{
    auto base_response = ostk::ObjectStoreResponse::create(request);
    if (!response->ok()) {
        base_response->on_error(
            {ostk::ObjectStoreErrorCode::ERROR,
             response->get_error().to_string()});
    }
    else {
        base_response->object() = response->m_object;
        base_response->set_object_found(response->object_found());
    }
    return base_response;
}