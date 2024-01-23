#include "HsmObjectStoreResponse.h"

namespace hestia {
HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request, const std::string& client_id) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(request, client_id)
{
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request,
    HsmObjectStoreResponse::Ptr hsm_child_response) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(
        request, hsm_child_response->get_store_id())
{
    m_object = hsm_child_response->object();
}

HsmObjectStoreResponse::HsmObjectStoreResponse(
    const BaseObjectStoreRequest& request,
    ObjectStoreResponse::Ptr child_response) :
    BaseObjectStoreResponse<HsmObjectStoreErrorCode>(
        request, child_response->get_store_id())
{
    m_object = child_response->object();
}

HsmObjectStoreResponse::Ptr HsmObjectStoreResponse::create(
    const BaseObjectStoreRequest& request, const std::string& client_id)
{
    return std::make_unique<HsmObjectStoreResponse>(request, client_id);
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
    auto base_response =
        ObjectStoreResponse::create(request, response->get_store_id());
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