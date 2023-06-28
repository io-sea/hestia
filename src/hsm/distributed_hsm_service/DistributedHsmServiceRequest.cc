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

DistributedHsmServiceRequest::DistributedHsmServiceRequest(
    const Metadata& query) :
    MethodRequest<DistributedHsmServiceRequestMethod>(
        DistributedHsmServiceRequestMethod::LIST),
    m_query(query)

{
}

const Metadata& DistributedHsmServiceRequest::query() const
{
    return m_query;
}

std::string DistributedHsmServiceRequest::method_as_string() const
{
    switch (m_method) {
        case DistributedHsmServiceRequestMethod::PUT:
            return "PUT";
        case DistributedHsmServiceRequestMethod::GET:
            return "GET";
        case DistributedHsmServiceRequestMethod::LIST:
            return "LIST";
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

DistributedHsmServiceResponse::DistributedHsmServiceResponse(
    const DistributedHsmServiceRequest& request,
    std::unique_ptr<CrudResponse<HsmNode, CrudErrorCode>> crud_response) :
    Response<DistributedHsmServiceErrorCode>(request)
{
    m_crud_response = std::move(crud_response);
    if (!m_crud_response->ok()) {
        on_error(
            {DistributedHsmServiceErrorCode::ERROR,
             "Error in crud response: "
                 + m_crud_response->get_error().to_string()});
    }
}

DistributedHsmServiceResponse::Ptr DistributedHsmServiceResponse::create(
    const DistributedHsmServiceRequest& request,
    std::unique_ptr<CrudResponse<HsmNode, CrudErrorCode>> crud_response)
{
    return std::make_unique<DistributedHsmServiceResponse>(
        request, std::move(crud_response));
}

DistributedHsmServiceResponse::Ptr DistributedHsmServiceResponse::create(
    const DistributedHsmServiceRequest& request)
{
    return std::make_unique<DistributedHsmServiceResponse>(request);
}

const HsmNode& DistributedHsmServiceResponse::item() const
{
    if (m_crud_response) {
        return m_crud_response->item();
    }

    return m_item;
}

const std::vector<HsmNode>& DistributedHsmServiceResponse::items() const
{
    if (m_crud_response) {
        return m_crud_response->items();
    }
    return m_items;
}

std::vector<HsmNode>& DistributedHsmServiceResponse::items()
{
    return m_items;
}
}  // namespace hestia