#include "HsmObjectStoreRequest.h"

#include <sstream>

namespace hestia {
HsmObjectStoreRequest::HsmObjectStoreRequest(
    const std::string& object_id, HsmObjectStoreRequestMethod method) :
    MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(object_id, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const StorageObject& object, HsmObjectStoreRequestMethod method) :
    MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(object, method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const Uuid& object_id, HsmObjectStoreRequestMethod method) :
    MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        object_id.to_string(), method)
{
}

HsmObjectStoreRequest::HsmObjectStoreRequest(
    const ObjectStoreRequest& request) :
    MethodObjectStoreRequest<HsmObjectStoreRequestMethod>(
        request, from_base_method(request.method()))
{
}

ObjectStoreRequest HsmObjectStoreRequest::to_base_request(
    const HsmObjectStoreRequest& reqeust)
{
    if (reqeust.is_hsm_only_request()) {
        throw std::runtime_error(
            "Attempted to convert a HSM operation to base type.");
    }

    ObjectStoreRequest base_request(
        reqeust.object(), to_base_method(reqeust.method()));
    base_request.set_extent(reqeust.extent());
    return base_request;
}

HsmObjectStoreRequestMethod HsmObjectStoreRequest::from_base_method(
    ObjectStoreRequestMethod method)
{
    switch (method) {
        case ObjectStoreRequestMethod::GET:
            return HsmObjectStoreRequestMethod::GET;
        case ObjectStoreRequestMethod::PUT:
            return HsmObjectStoreRequestMethod::PUT;
        case ObjectStoreRequestMethod::REMOVE:
            return HsmObjectStoreRequestMethod::REMOVE;
        case ObjectStoreRequestMethod::LIST:
        case ObjectStoreRequestMethod::EXISTS:
        case ObjectStoreRequestMethod::CUSTOM:
        default:
            throw std::runtime_error(
                "Given unsupported base request type for hsm request type conversion.");
    }
}

ObjectStoreRequestMethod HsmObjectStoreRequest::to_base_method(
    HsmObjectStoreRequestMethod method)
{
    switch (method) {
        case HsmObjectStoreRequestMethod::GET:
            return ObjectStoreRequestMethod::GET;
        case HsmObjectStoreRequestMethod::PUT:
            return ObjectStoreRequestMethod::PUT;
        case HsmObjectStoreRequestMethod::REMOVE:
            return ObjectStoreRequestMethod::REMOVE;
        case HsmObjectStoreRequestMethod::REMOVE_ALL:
        case HsmObjectStoreRequestMethod::COPY:
        case HsmObjectStoreRequestMethod::MOVE:
        case HsmObjectStoreRequestMethod::EXISTS:
        default:
            return ObjectStoreRequestMethod::CUSTOM;
    }
}

bool HsmObjectStoreRequest::is_copy_or_move_request(
    HsmObjectStoreRequestMethod method)
{
    return method == HsmObjectStoreRequestMethod::MOVE
           || method == HsmObjectStoreRequestMethod::COPY;
}

bool HsmObjectStoreRequest::is_hsm_supported_method(
    ObjectStoreRequestMethod op_type)
{
    return op_type == ObjectStoreRequestMethod::GET
           || op_type == ObjectStoreRequestMethod::PUT
           || op_type == ObjectStoreRequestMethod::REMOVE;
}

bool HsmObjectStoreRequest::is_hsm_only_request() const
{
    return is_copy_or_move_request(m_method);
}

bool HsmObjectStoreRequest::is_source_tier_request() const
{
    return method() == HsmObjectStoreRequestMethod::GET
           || method() == HsmObjectStoreRequestMethod::EXISTS
           || method() == HsmObjectStoreRequestMethod::REMOVE;
}

bool HsmObjectStoreRequest::is_single_tier_request() const
{
    return is_source_tier_request()
           || method() == HsmObjectStoreRequestMethod::PUT;
}

std::string HsmObjectStoreRequest::method_as_string() const
{
    return to_string(m_method);
}

std::string HsmObjectStoreRequest::to_string() const
{
    std::stringstream sstr;
    sstr << "ObjectId: " << object().id() << " | ";
    sstr << "Operation: " << to_string(m_method) << " | ";
    sstr << "Extent: " << extent().to_string() << " | ";
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
        case HsmObjectStoreRequestMethod::REMOVE_ALL:
            return "REMOVE_ALL";
        case HsmObjectStoreRequestMethod::EXISTS:
            return "EXISTS";
        default:
            return "UNKNOWN";
    }
}

const std::string& HsmObjectStoreRequest::source_tier() const
{
    return m_source_tier;
}

const std::string& HsmObjectStoreRequest::target_tier() const
{
    return m_target_tier;
}
}  // namespace hestia
