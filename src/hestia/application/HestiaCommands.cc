#include "HestiaCommands.h"

#include "ConsoleInterface.h"

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

HestiaClientCommand::IoFormat HestiaClientCommand::io_format_from_string(
    const std::string& format)
{
    return IoFormat_enum_string_converter().init().from_string(format);
}

bool HestiaClientCommand::expects_id(IoFormat format)
{
    return format == IoFormat::ID || format == IoFormat::IDs
           || format == IoFormat::IDS_ATTRS_JSON
           || format == IoFormat::IDS_ATTRS_KV_PAIR;
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

std::pair<HestiaClientCommand::IoFormat, CrudAttributes::Format>
HestiaClientCommand::parse_create_update_inputs(
    VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    IConsoleInterface* console) const
{
    IoFormat input_format{IoFormat::NONE};
    if (!m_input_format.empty()) {
        input_format = io_format_from_string(m_input_format);
    }

    IoFormat output_format{IoFormat::IDs};
    if (!m_output_format.empty()) {
        output_format = io_format_from_string(m_output_format);
    }

    CrudIdentifier::InputFormat id_format{CrudIdentifier::InputFormat::ID};
    if (expects_id(input_format) && !m_id_format.empty()) {
        id_format = CrudIdentifier::input_format_from_string(m_id_format);
    }

    std::string attribute_buffer;
    if (input_format == IoFormat::ID && !m_body.empty()) {
        CrudIdentifier::parse(m_body, id_format, ids);
    }
    else if (input_format != IoFormat::NONE) {
        std::string line;
        bool in_ids = expects_id(input_format);
        while (console->console_read(line)) {
            if (line.empty()) {
                in_ids = false;
            }
            else if (in_ids) {
                CrudIdentifier id;
                id.from_buffer(line, id_format);
                ids.push_back(id);
            }
            else {
                attribute_buffer += line;
            }
        }
    }

    attributes.set_buffer(attribute_buffer);

    CrudAttributes::Format attribute_input_format{CrudAttributes::Format::NONE};
    if (input_format == IoFormat::ATTRS_JSON
        || input_format == IoFormat::IDS_ATTRS_JSON) {
        attribute_input_format = CrudAttributes::Format::JSON;
    }
    else if (
        input_format == IoFormat::ATTRS_KV_PAIR
        || input_format == IoFormat::IDS_ATTRS_KV_PAIR) {
        attribute_input_format = CrudAttributes::Format::KV_PAIR;
    }
    attributes.set_format(attribute_input_format);

    CrudAttributes::Format attribute_output_format{
        CrudAttributes::Format::NONE};
    if (output_format == IoFormat::ATTRS_JSON
        || output_format == IoFormat::IDS_ATTRS_JSON) {
        attribute_output_format = CrudAttributes::Format::JSON;
    }
    else if (
        output_format == IoFormat::ATTRS_KV_PAIR
        || output_format == IoFormat::IDS_ATTRS_KV_PAIR) {
        attribute_output_format = CrudAttributes::Format::KV_PAIR;
    }
    return {output_format, attribute_output_format};
}

}  // namespace hestia