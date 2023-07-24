#include "HsmActionResponse.h"

namespace hestia {

HsmActionResponse::HsmActionResponse(
    const BaseRequest& request, HsmObjectStoreResponse::Ptr response) :
    Response<HsmActionErrorCode>(request),
    m_object_store_response(std::move(response))
{
    if (m_object_store_response != nullptr && !m_object_store_response->ok()) {
        on_error(
            {HsmActionErrorCode::ERROR,
             m_object_store_response->get_base_error().to_string()});
    }
}

HsmActionResponse::Ptr HsmActionResponse::create(
    const BaseRequest& request, HsmObjectStoreResponse::Ptr response)
{
    return std::make_unique<HsmActionResponse>(request, std::move(response));
}

}  // namespace hestia
