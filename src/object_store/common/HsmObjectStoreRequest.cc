#include "HsmObjectStoreRequest.h"

#include <sstream>

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const std::string& object_id, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        object_id, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::StorageObject& object, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(object, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::Uuid& object_id, HsmObjectStoreRequestMethod method) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        object_id.toString(), method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ostk::ObjectStoreRequest& request) :
    ostk::MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        request, from_base_method(request.method()))
{
}

ostk::ObjectStoreRequest HsmObjectStoreRequest::to_base_request(
    const HsmObjectStoreRequest& reqeust)
{
    if (reqeust.is_hsm_only_request()) {
        throw std::runtime_error(
            "Attempted to convert a HSM operation to base type.");
    }

    ostk::ObjectStoreRequest base_request(
        reqeust.object(), to_base_method(reqeust.method()));
    base_request.setExtent(reqeust.extent());
    return base_request;
}

HsmObjectStoreRequestMethod HsmObjectStoreRequest::from_base_method(
    ostk::ObjectStoreRequestMethod method)
{
    switch (method) {
        case ostk::ObjectStoreRequestMethod::GET:
            return HsmObjectStoreRequestMethod::GET;
        case ostk::ObjectStoreRequestMethod::PUT:
            return HsmObjectStoreRequestMethod::PUT;
        case ostk::ObjectStoreRequestMethod::REMOVE:
            return HsmObjectStoreRequestMethod::REMOVE;
        default:
            throw std::runtime_error(
                "Given unsupported base request type for hsm request type conversion.");
    }
}

ostk::ObjectStoreRequestMethod HsmObjectStoreRequest::to_base_method(
    HsmObjectStoreRequestMethod method)
{
    switch (method) {
        case HsmObjectStoreRequestMethod::GET:
            return ostk::ObjectStoreRequestMethod::GET;
        case HsmObjectStoreRequestMethod::PUT:
            return ostk::ObjectStoreRequestMethod::PUT;
        case HsmObjectStoreRequestMethod::REMOVE:
            return ostk::ObjectStoreRequestMethod::REMOVE;
        default:
            return ostk::ObjectStoreRequestMethod::CUSTOM;
    }
}

bool HsmObjectStoreRequest::is_copy_or_move_request(
    HsmObjectStoreRequestMethod method)
{
    return method == HsmObjectStoreRequestMethod::MOVE
           || method == HsmObjectStoreRequestMethod::COPY;
}

bool HsmObjectStoreRequest::is_hsm_supported_method(
    ostk::ObjectStoreRequestMethod op_type)
{
    return op_type == ostk::ObjectStoreRequestMethod::GET
           || op_type == ostk::ObjectStoreRequestMethod::PUT
           || op_type == ostk::ObjectStoreRequestMethod::REMOVE;
}

bool HsmObjectStoreRequest::is_hsm_only_request() const
{
    return is_copy_or_move_request(mMethod);
}

std::string HsmObjectStoreRequest::methodAsString() const
{
    return to_string(mMethod);
}

std::string HsmObjectStoreRequest::to_string() const
{
    std::stringstream sstr;
    sstr << "ObjectId: " << object().id() << " | ";
    sstr << "Operation: " << to_string(mMethod) << " | ";
    sstr << "Extent: " << extent().toString() << " | ";
    sstr << "Source Tier: " << m_source_tier << " | ";
    sstr << "mTargetTier Tier: " << m_target_tier;
    return sstr.str();
}

std::string HsmObjectStoreRequest::to_string(
    HsmObjectStoreRequestMethod opt_type)
{
    switch (opt_type) {
        case HsmObjectStoreRequestMethod::PUT:
            return "PUT";
        case HsmObjectStoreRequestMethod::GET:
            return "GET";
        case HsmObjectStoreRequestMethod::REMOVE:
            return "REMOVE";
        case HsmObjectStoreRequestMethod::COPY:
            return "COPY";
        case HsmObjectStoreRequestMethod::MOVE:
            return "MOVE";
        default:
            return "UNKNOWN";
    }
}

uint8_t HsmObjectStoreRequest::source_tier() const
{
    return m_source_tier;
}

uint8_t HsmObjectStoreRequest::target_tier() const
{
    return m_target_tier;
}