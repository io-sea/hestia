#include "HsmActionResponse.h"

namespace hestia {

HsmActionResponse::HsmActionResponse(
    const BaseRequest& request,
    const HsmAction& action,
    HsmObjectStoreResponse::Ptr response) :
    Response<HsmActionErrorCode>(request),
    m_action(action),
    m_object_store_response(std::move(response))
{
    if (m_object_store_response != nullptr && !m_object_store_response->ok()) {
        on_error(
            {HsmActionErrorCode::ERROR,
             m_object_store_response->get_base_error().to_string()});
    }
}

HsmActionResponse::Ptr HsmActionResponse::create(
    const BaseRequest& request,
    const HsmAction& action,
    HsmObjectStoreResponse::Ptr response)
{
    return std::make_unique<HsmActionResponse>(
        request, action, std::move(response));
}

void HsmActionResponse::set_redirect_location(const std::string& redirect)
{
    m_redirect_location = redirect;
}

const std::string& HsmActionResponse::get_redirect_location() const
{
    return m_redirect_location;
}

HsmAction& HsmActionResponse::action()
{
    return m_action;
}

const HsmAction& HsmActionResponse::get_action() const
{
    return m_action;
}

}  // namespace hestia
