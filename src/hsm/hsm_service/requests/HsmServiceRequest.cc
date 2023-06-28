#include "HsmServiceRequest.h"

#include <sstream>

namespace hestia {
HsmServiceRequest::HsmServiceRequest(
    const Uuid& subject_id,
    HsmServiceRequestSubject subject,
    HsmServiceRequestMethod method) :
    MethodRequest<HsmServiceRequestMethod>(method), m_subject(subject)
{
    if (subject == HsmServiceRequestSubject::OBJECT) {
        m_object = HsmObject(subject_id);
    }
    else {
        m_dataset = Dataset(subject_id);
    }
}

HsmServiceRequest::HsmServiceRequest(
    const HsmObject& object, HsmServiceRequestMethod method) :
    MethodRequest<HsmServiceRequestMethod>(method), m_object(object)
{
}

HsmServiceRequest::HsmServiceRequest(
    const Dataset& dataset, HsmServiceRequestMethod method) :
    MethodRequest<HsmServiceRequestMethod>(method),
    m_dataset(dataset),
    m_subject(HsmServiceRequestSubject::DATASET)
{
}

HsmServiceRequest::HsmServiceRequest(
    const uint8_t& tier,
    HsmServiceRequestSubject subject,
    HsmServiceRequestMethod method) :
    MethodRequest<HsmServiceRequestMethod>(method),
    m_subject(subject),
    m_tier(tier)
{
}

HsmServiceRequest::HsmServiceRequest(
    HsmServiceRequestSubject subject, HsmServiceRequestMethod method) :
    MethodRequest<HsmServiceRequestMethod>(method), m_subject(subject)
{
}

std::string HsmServiceRequest::method_as_string() const
{
    switch (m_method) {
        case HsmServiceRequestMethod::CREATE:
            return "CREATE";
        case HsmServiceRequestMethod::PUT:
            return "PUT";
        case HsmServiceRequestMethod::GET:
            return "GET";
        case HsmServiceRequestMethod::EXISTS:
            return "EXISTS";
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
        case HsmServiceRequestMethod::LIST_ATTRIBUTES:
            return "LIST_ATTRIBUTES";
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

const HsmObject& HsmServiceRequest::object() const
{
    return m_object;
}

HsmObject& HsmServiceRequest::object()
{
    return m_object;
}

const uint8_t& HsmServiceRequest::tier() const
{
    return m_tier;
}

uint8_t& HsmServiceRequest::tier()
{
    return m_tier;
}

uint8_t HsmServiceRequest::source_tier() const
{
    return m_source_tier;
}

uint8_t HsmServiceRequest::target_tier() const
{
    return m_target_tier;
}

const std::string& HsmServiceRequest::query() const
{
    return m_query;
}

const Extent& HsmServiceRequest::extent() const
{
    return m_extent;
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
