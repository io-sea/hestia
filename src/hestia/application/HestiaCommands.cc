#include "HestiaCommands.h"

namespace hestia {

void HestiaClientCommand::set_hsm_subject(const std::string& subject)
{
    m_subject.m_type = HestiaType::Type::HSM;
    m_subject.m_hsm_type =
        HsmItem::Type_enum_string_converter().init().from_string(subject);
}

void HestiaClientCommand::set_system_subject(const std::string& subject)
{
    m_subject.m_type        = HestiaType::Type::SYSTEM;
    m_subject.m_system_type = subject == "user" ?
                                  HestiaType::SystemType::USER :
                                  HestiaType::SystemType::HSM_NODE;
}

void HestiaClientCommand::set_crud_method(const std::string& method)
{
    m_action.set_action(HsmAction::Action::CRUD);
    m_crud_method =
        CrudMethod_enum_string_converter().init().from_string(method);
}

void HestiaClientCommand::set_hsm_action(HsmAction::Action action)
{
    m_action.set_action(action);
    m_action.set_source_tier(m_source_tier);
    m_action.set_target_tier(m_target_tier);
}

bool HestiaClientCommand::is_crud_method() const
{
    return m_action.is_crud_method();
}

bool HestiaClientCommand::is_create_method() const
{
    return m_crud_method == CrudMethod::CREATE;
}

bool HestiaClientCommand::is_read_method() const
{
    return m_crud_method == CrudMethod::READ;
}

bool HestiaClientCommand::is_identify_method() const
{
    return m_crud_method == CrudMethod::IDENTIFY;
}

bool HestiaClientCommand::is_update_method() const
{
    return m_crud_method == CrudMethod::UPDATE;
}

bool HestiaClientCommand::is_remove_method() const
{
    return m_crud_method == CrudMethod::REMOVE;
}

bool HestiaClientCommand::is_data_put_action() const
{
    return m_action.is_data_put_action();
}

bool HestiaClientCommand::is_data_management_action() const
{
    return m_action.is_data_management_action();
}

bool HestiaClientCommand::is_data_io_action() const
{
    return m_action.is_data_io_action();
}

std::vector<std::string> HestiaClientCommand::get_crud_methods() const
{
    std::vector<std::string> methods;
    CrudMethod_enum_string_converter converter;
    converter.init();
    for (const auto& method : BaseCrudRequest::s_all_methods) {
        methods.push_back(converter.to_string(method));
    }
    return methods;
}

std::vector<std::string> HestiaClientCommand::get_action_subjects() const
{
    return {"object", "dataset"};
}

std::vector<HsmAction::Action> HestiaClientCommand::get_subject_actions(
    const std::string& subject) const
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

std::vector<std::string> HestiaClientCommand::get_hsm_subjects() const
{
    std::vector<std::string> subjects;
    for (const auto& subject : HsmItem::get_all_items()) {
        subjects.push_back(HsmItem::to_name(subject));
    }
    return subjects;
}

std::vector<std::string> HestiaClientCommand::get_system_subjects() const
{
    return {"user", "hsm_node"};
}

}  // namespace hestia