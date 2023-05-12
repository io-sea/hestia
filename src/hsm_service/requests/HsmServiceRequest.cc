#include "HsmServiceRequest.h"

#include <sstream>

namespace hestia {
HsmServiceRequest::HsmServiceRequest(
    const hestia::Uuid& object_id, HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method)
{
    m_object = hestia::StorageObject(object_id.to_string());
}

HsmServiceRequest::HsmServiceRequest(
    const hestia::StorageObject& object, HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method), m_object(object)
{
}

HsmServiceRequest::HsmServiceRequest(HsmServiceRequestMethod method) :
    hestia::MethodRequest<HsmServiceRequestMethod>(method)
{
}

std::string HsmServiceRequest::method_as_string() const
{
    switch (m_method) {
        case HsmServiceRequestMethod::PUT:
            return "PUT";
        case HsmServiceRequestMethod::GET:
            return "GET";
        case HsmServiceRequestMethod::GET_TIERS:
            return "GET_TIERS";
        case HsmServiceRequestMethod::REMOVE:
            return "REMOVE";
        case HsmServiceRequestMethod::REMOVE_ALL:
            return "REMOVE_ALL";
        case HsmServiceRequestMethod::COPY:
            return "COPY";
        case HsmServiceRequestMethod::MOVE:
            return "MOVE";
        case HsmServiceRequestMethod::LIST:
            return "LIST";
        case HsmServiceRequestMethod::LIST_TIERS:
            return "LIST_TIERS";
        default:
            return "UNKNOWN";
    }
}

void HsmServiceRequest::set_extent(const hestia::Extent& extent)
{
    m_extent = extent;
}

void HsmServiceRequest::set_target_tier(uint8_t tier)
{
    m_target_tier = tier;
}

void HsmServiceRequest::set_source_tier(uint8_t tier)
{
    m_source_tier = tier;
}

void HsmServiceRequest::set_should_put_overwrite(bool overwrite)
{
    m_overwrite_if_existing = overwrite;
}

bool HsmServiceRequest::should_overwrite_put() const
{
    return m_overwrite_if_existing;
}

void HsmServiceRequest::set_query(const std::string& query)
{
    m_query = query;
}

const hestia::StorageObject& HsmServiceRequest::object() const
{
    return m_object;
}

std::string HsmServiceRequest::to_string() const
{
    std::stringstream sstr;
    sstr << "HsmServiceRequest: [";
    sstr << "Method:" << method_as_string() << ", ";
    sstr << "Object:[" << m_object.to_string() << "], ";
    sstr << "Source Tier:" << std::to_string(m_source_tier) << ", ";
    sstr << "Target Tier:" << std::to_string(m_target_tier) << "]";
    return sstr.str();
}

}  // namespace hestia