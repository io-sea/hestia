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
    mMiddlewareResponse(std::move(middlewareResponse))
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
    HsmMiddlewareResponse::Ptr middlewareResponse)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(middlewareResponse));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsmChildResponse)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(hsmChildResponse));
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const ostk::BaseObjectStoreRequest& request,
    ostk::ObjectStoreResponse::Ptr childResponse)
{
    return std::make_unique<HsmObjectStoreResponse>(
        request, std::move(childResponse));
}

ostk::ObjectStoreResponse::Ptr HsmObjectStoreResponse::to_base_response(
    const ostk::BaseObjectStoreRequest& request,
    const HsmObjectStoreResponse* response)
{
    auto base_response = ostk::ObjectStoreResponse::Create(request);
    if (!response->ok()) {
        base_response->onError(
            {ostk::ObjectStoreErrorCode::ERROR,
             response->getError().toString()});
    }
    else {
        base_response->object() = response->mObject;
        base_response->setObjectFound(response->objectFound());
    }
    return base_response;
}