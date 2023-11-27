#include "HsmAction.h"

namespace hestia {
HsmAction::HsmAction() :
    HsmItem(HsmItem::Type::ACTION), OwnableModel(HsmItem::hsm_action_name, {})
{
    init();
}

HsmAction::HsmAction(HsmItem::Type subject, HsmAction::Action action) :
    HsmItem(HsmItem::Type::ACTION), OwnableModel(HsmItem::hsm_action_name, {})
{
    m_action.init_value(action);
    m_subject.init_value(subject);
    init();
}

HsmAction::HsmAction(const HsmAction& other) :
    HsmItem(other), OwnableModel(other)
{
    *this = other;
}

HsmAction& HsmAction::operator=(const HsmAction& other)
{
    if (this != &other) {
        OwnableModel::operator=(other);
        m_action            = other.m_action;
        m_subject           = other.m_subject;
        m_status            = other.m_status;
        m_to_transfer       = other.m_to_transfer;
        m_offset            = other.m_offset;
        m_transferred       = other.m_transferred;
        m_source_tier       = other.m_source_tier;
        m_target_tier       = other.m_target_tier;
        m_source_tier_id    = other.m_source_tier_id;
        m_target_tier_id    = other.m_target_tier_id;
        m_subject_key       = other.m_subject_key;
        m_is_request        = other.m_is_request;
        m_status_message    = other.m_status_message;
        m_progress_interval = other.m_progress_interval;
        init();
    }
    return *this;
}

void HsmAction::init()
{
    register_scalar_field(&m_action);
    register_scalar_field(&m_subject);
    register_scalar_field(&m_status);
    register_scalar_field(&m_to_transfer);
    register_scalar_field(&m_offset);
    register_scalar_field(&m_transferred);
    register_scalar_field(&m_source_tier);
    register_scalar_field(&m_target_tier);
    register_scalar_field(&m_source_tier_id);
    register_scalar_field(&m_target_tier_id);
    register_scalar_field(&m_subject_key);
    register_scalar_field(&m_is_request);
    register_scalar_field(&m_status_message);
    register_scalar_field(&m_progress_interval);
}

bool HsmAction::is_crud_method() const
{
    return m_action.get_value() == HsmAction::Action::CRUD;
}

std::string HsmAction::get_type()
{
    return HsmItem::hsm_action_name;
}

std::string HsmAction::get_action_as_string() const
{
    return m_action.value_as_string();
}

const std::string& HsmAction::get_source_tier_id() const
{
    return m_source_tier_id.get_value();
}

const std::string& HsmAction::get_target_tier_id() const
{
    return m_target_tier_id.get_value();
}

bool HsmAction::has_source_tier_id() const
{
    return !m_source_tier_id.get_value().empty();
}

bool HsmAction::has_target_tier_id() const
{
    return !m_target_tier_id.get_value().empty();
}

void HsmAction::set_source_tier(uint8_t tier)
{
    m_source_tier.update_value(tier);
}

void HsmAction::set_target_tier(uint8_t tier)
{
    m_target_tier.update_value(tier);
}

void HsmAction::set_action(Action action)
{
    m_action.update_value(action);
}

std::size_t HsmAction::get_progress_interval() const
{
    return m_progress_interval.get_value();
}

void HsmAction::on_error(const std::string& message)
{
    m_status.update_value(HsmAction::Status::ERROR);
    m_status_message.update_value(message);
}

void HsmAction::on_finished_ok(std::size_t bytes_transferred)
{
    m_status.update_value(HsmAction::Status::FINISHED_OK);
    m_transferred.update_value(bytes_transferred);
}

bool HsmAction::is_data_management_action() const
{
    return (m_action.get_value() == HsmAction::Action::COPY_DATA)
           || (m_action.get_value() == HsmAction::Action::MOVE_DATA)
           || (m_action.get_value() == HsmAction::Action::RELEASE_DATA);
}

bool HsmAction::is_data_io_action() const
{
    return is_data_put_action()
           || (m_action.get_value() == HsmAction::Action::GET_DATA);
}

bool HsmAction::is_data_put_action() const
{
    return m_action.get_value() == HsmAction::Action::PUT_DATA;
}

bool HsmAction::has_action() const
{
    return m_action.get_value() != Action::NONE;
}

const std::string& HsmAction::get_subject_key() const
{
    return m_subject_key.get_value();
}

HsmItem::Type HsmAction::get_subject() const
{
    return m_subject.get_value();
}

HsmAction::Action HsmAction::get_action() const
{
    return m_action.get_value();
}

uint8_t HsmAction::get_source_tier() const
{
    return m_source_tier.get_value();
}

uint8_t HsmAction::get_target_tier() const
{
    return m_target_tier.get_value();
}

std::size_t HsmAction::get_offset() const
{
    return m_offset.get_value();
}

std::size_t HsmAction::get_size() const
{
    return m_to_transfer.get_value();
}

std::size_t HsmAction::get_num_transferred() const
{
    return m_transferred.get_value();
}

void HsmAction::set_offset(std::size_t offset)
{
    m_offset.update_value(offset);
}

void HsmAction::set_subject_key(const std::string& key)
{
    m_subject_key.update_value(key);
}

void HsmAction::set_subject(HsmItem::Type subject)
{
    m_subject.update_value(subject);
}

void HsmAction::set_size(std::size_t size)
{
    m_to_transfer.update_value(size);
}

void HsmAction::set_source_tier_id(const std::string& id)
{
    m_source_tier_id.update_value(id);
}

void HsmAction::set_target_tier_id(const std::string& id)
{
    m_target_tier_id.update_value(id);
}

void HsmAction::set_num_transferred(std::size_t size)
{
    m_transferred.update_value(size);
}

std::vector<std::string> HsmAction::get_action_subjects()
{
    return {"object", "dataset"};
}

std::vector<HsmAction::Action> HsmAction::get_subject_actions(
    const std::string& subject)
{
    if (subject == "object") {
        return {
            HsmAction::Action::PUT_DATA, HsmAction::Action::GET_DATA,
            HsmAction::Action::COPY_DATA, HsmAction::Action::MOVE_DATA,
            HsmAction::Action::RELEASE_DATA};
    }
    else if (subject == "dataset") {
        return {
            HsmAction::Action::COPY_DATA, HsmAction::Action::MOVE_DATA,
            HsmAction::Action::RELEASE_DATA};
    }
    return {};
}

}  // namespace hestia