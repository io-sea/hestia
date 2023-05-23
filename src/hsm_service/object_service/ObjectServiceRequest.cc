#include "ObjectServiceRequest.h"

#include <sstream>

namespace hestia {
ObjectServiceRequest::ObjectServiceRequest(
    const HsmObject& object, ObjectServiceRequestMethod method) :
    MethodRequest<ObjectServiceRequestMethod>(method), m_object(object)
{
}

std::string ObjectServiceRequest::method_as_string() const
{
    switch (m_method) {
        case ObjectServiceRequestMethod::PUT:
            return "PUT";
        case ObjectServiceRequestMethod::GET:
            return "GET";
        case ObjectServiceRequestMethod::EXISTS:
            return "EXISTS";
        case ObjectServiceRequestMethod::REMOVE:
            return "REMOVE";
        case ObjectServiceRequestMethod::LIST:
            return "LIST";
        default:
            return "UNKNOWN";
    }
}

std::string ObjectServiceRequest::to_string() const
{
    std::stringstream sstr;
    sstr << "ObjectServiceRequest: [";
    sstr << "Method:" << method_as_string() << ", ";
    sstr << "Object:[" << m_object.to_string() << "], ";
    return sstr.str();
}

ObjectServiceError::ObjectServiceError() :
    RequestError<ObjectServiceErrorCode>(ObjectServiceErrorCode::NO_ERROR, {})
{
}

ObjectServiceError::ObjectServiceError(
    ObjectServiceErrorCode code, const std::string& message) :
    RequestError<ObjectServiceErrorCode>(code, message)
{
}

std::string ObjectServiceError::code_as_string() const
{
    return code_to_string(m_code);
}

std::string ObjectServiceError::code_to_string(ObjectServiceErrorCode code)
{
    switch (code) {
        case ObjectServiceErrorCode::NO_ERROR:
            return "NO_ERROR";
        case ObjectServiceErrorCode::ERROR:
            return "ERROR";
        case ObjectServiceErrorCode::STL_EXCEPTION:
            return "ObjectServiceErrorCode";
        case ObjectServiceErrorCode::UNKNOWN_EXCEPTION:
            return "UNKNOWN_EXCEPTION";
        case ObjectServiceErrorCode::UNSUPPORTED_REQUEST_METHOD:
            return "UNSUPPORTED_REQUEST_METHOD";
        case ObjectServiceErrorCode::MAX_ERROR:
            return "MAX_ERROR";
        case ObjectServiceErrorCode::OBJECT_NOT_FOUND:
            return "BAD_PUT_OVERWRITE_COMBINATION";
        default:
            return "UNKOWN ERROR";
    }
}

ObjectServiceResponse::ObjectServiceResponse(
    const ObjectServiceRequest& request) :
    Response<ObjectServiceErrorCode>(request)
{
    m_object = request.object();
}

ObjectServiceResponse::Ptr ObjectServiceResponse::create(
    const ObjectServiceRequest& request)
{
    return std::make_unique<ObjectServiceResponse>(request);
}

const std::string& ObjectServiceResponse::query_result() const
{
    return m_query_result;
}

const HsmObject& ObjectServiceResponse::object() const
{
    return m_object;
}

const std::vector<HsmObject>& ObjectServiceResponse::objects() const
{
    return m_objects;
}

}  // namespace hestia
