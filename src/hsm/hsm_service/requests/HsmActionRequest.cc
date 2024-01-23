#include "HsmActionRequest.h"

#include <sstream>

namespace hestia {
HsmActionRequest::HsmActionRequest(
    const HsmAction& action,
    const CrudUserContext& user_context,
    HsmNodeInterface::Type interface) :
    BaseRequest(),
    MethodRequest<HsmAction::Action>(action.get_action()),
    m_interface(interface),
    m_user_context(user_context),
    m_action(action)
{
}

std::string HsmActionRequest::method_as_string() const
{
    switch (m_method) {
        case HsmAction::Action::CRUD:
            return "CRUD";
        case HsmAction::Action::PUT_DATA:
            return "PUT_DATA";
        case HsmAction::Action::GET_DATA:
            return "GET_DATA";
        case HsmAction::Action::COPY_DATA:
            return "COPY_DATA";
        case HsmAction::Action::MOVE_DATA:
            return "MOVE_DATA";
        case HsmAction::Action::RELEASE_DATA:
            return "RELEASE_DATA";
        case HsmAction::Action::NONE:
        default:
            return "UNKNOWN";
    }
}

std::string HsmActionRequest::subject_as_string() const
{
    return HsmItem::to_name(get_subject());
}

const CrudUserContext& HsmActionRequest::get_user_context() const
{
    return m_user_context;
}

HsmNodeInterface::Type HsmActionRequest::get_interface() const
{
    return m_interface;
}

void HsmActionRequest::set_interface(HsmNodeInterface::Type interface)
{
    m_interface = interface;
}

const HsmAction& HsmActionRequest::get_action() const
{
    return m_action;
}

HsmItem::Type HsmActionRequest::get_subject() const
{
    return m_action.get_subject();
}

uint8_t HsmActionRequest::source_tier() const
{
    return m_action.get_source_tier();
}

uint8_t HsmActionRequest::target_tier() const
{
    return m_action.get_target_tier();
}

Extent HsmActionRequest::extent() const
{
    return {m_action.get_offset(), m_action.get_size()};
}

bool HsmActionRequest::is_release_data_action() const
{
    return m_action.get_action() == HsmAction::Action::RELEASE_DATA;
}

bool HsmActionRequest::is_copy_data_action() const
{
    return m_action.get_action() == HsmAction::Action::COPY_DATA;
}

bool HsmActionRequest::is_move_data_action() const
{
    return m_action.get_action() == HsmAction::Action::MOVE_DATA;
}

bool HsmActionRequest::is_put_data_action() const
{
    return m_action.get_action() == HsmAction::Action::PUT_DATA;
}

bool HsmActionRequest::is_get_data_action() const
{
    return m_action.get_action() == HsmAction::Action::GET_DATA;
}

bool HsmActionRequest::is_copy_or_move_data_action() const
{
    return is_copy_data_action() || is_move_data_action();
}

const std::string& HsmActionRequest::source_tier_id() const
{
    return m_action.get_source_tier_id();
}

const std::string& HsmActionRequest::target_tier_id() const
{
    return m_action.get_target_tier_id();
}

std::string HsmActionRequest::to_string() const
{
    std::stringstream sstr;
    sstr << "HsmActionRequest: [";
    sstr << "Method:" << method_as_string() << ", ";
    sstr << "Subject:" << subject_as_string() << "]";
    return sstr.str();
}

}  // namespace hestia
