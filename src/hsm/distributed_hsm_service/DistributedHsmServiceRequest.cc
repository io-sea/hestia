#include "DistributedHsmServiceRequest.h"

namespace hestia {

DistributedHsmServiceRequest::DistributedHsmServiceRequest(
    const HsmNode& item, DistributedHsmServiceRequestMethod method) :
    MethodRequest<DistributedHsmServiceRequestMethod>(method), m_item(item)
{
}

DistributedHsmServiceRequest::DistributedHsmServiceRequest(
    DistributedHsmServiceRequestMethod method) :
    MethodRequest<DistributedHsmServiceRequestMethod>(method)
{
}

std::string DistributedHsmServiceRequest::method_as_string() const
{
    switch (m_method) {
        case DistributedHsmServiceRequestMethod::PUT:
            return "PUT";
        case DistributedHsmServiceRequestMethod::GET:
            return "GET";
        default:
            return "UNKNOWN";
    }
}

const HsmNode& DistributedHsmServiceRequest::item() const
{
    return m_item;
}

DistributedHsmServiceError::DistributedHsmServiceError() :
    RequestError<DistributedHsmServiceErrorCode>(
        DistributedHsmServiceErrorCode::NO_ERROR, {})
{
}

DistributedHsmServiceError::DistributedHsmServiceError(
    DistributedHsmServiceErrorCode code, const std::string& message) :
    RequestError<DistributedHsmServiceErrorCode>(code, message)
{
}

std::string DistributedHsmServiceError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string DistributedHsmServiceError::code_to_string(
    DistributedHsmServiceErrorCode code)
{
    switch (code) {
        case DistributedHsmServiceErrorCode::NO_ERROR:
            return "NO_ERROR";
        case DistributedHsmServiceErrorCode::ERROR:
            return "ERROR";
        case DistributedHsmServiceErrorCode::STL_EXCEPTION:
            return "STL_EXCEPTION";
        case DistributedHsmServiceErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case DistributedHsmServiceErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case DistributedHsmServiceErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        case DistributedHsmServiceErrorCode::OBJECT_NOT_FOUND:
            return "BAD_PUT_OVERWRITE_COMBINATION";
        case DistributedHsmServiceErrorCode::BAD_PUT_OVERWRITE_COMBINATION:
            return "MAX_ERROR";
        default:
            return "UNKOWN ERROR";
    }
}

DistributedHsmServiceResponse::DistributedHsmServiceResponse(
    const DistributedHsmServiceRequest& request) :
    Response<DistributedHsmServiceErrorCode>(request)
{
}

DistributedHsmServiceResponse::Ptr DistributedHsmServiceResponse::create(
    const DistributedHsmServiceRequest& request)
{
    return std::make_unique<DistributedHsmServiceResponse>(request);
}

const HsmNode& DistributedHsmServiceResponse::item() const
{
    return m_item;
}

const std::vector<HsmNode>& DistributedHsmServiceResponse::items() const
{
    return m_items;
}

std::vector<HsmNode>& DistributedHsmServiceResponse::items()
{
    return m_items;
}
}  // namespace hestia