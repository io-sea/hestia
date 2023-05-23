#include "HsmServiceResponse.h"

namespace hestia {
HsmServiceResponse::HsmServiceResponse(const HsmServiceRequest& request) :
    hestia::Response<HsmServiceErrorCode>(request)
{
}

HsmServiceResponse::HsmServiceResponse(
    const HsmServiceRequest& request, BaseResponse::Ptr child_response) :
    hestia::Response<HsmServiceErrorCode>(request),
    m_child_response(std::move(child_response))
{
    if (!m_child_response->ok()) {
        on_error(
            {HsmServiceErrorCode::ERROR,
             m_child_response->get_base_error().to_string()});
    }
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request)
{
    return std::make_unique<HsmServiceResponse>(request);
}

HsmServiceResponse::Ptr HsmServiceResponse::create(
    const HsmServiceRequest& request, BaseResponse::Ptr child_response)
{
    return std::make_unique<HsmServiceResponse>(
        request, std::move(child_response));
}

const std::string& HsmServiceResponse::query_result() const
{
    return m_query_result;
}

const std::vector<hestia::Uuid>& HsmServiceResponse::objects() const
{
    return m_object_ids;
}

const std::vector<uint8_t>& HsmServiceResponse::tiers() const
{
    return m_tiers;
}
}  // namespace hestia
