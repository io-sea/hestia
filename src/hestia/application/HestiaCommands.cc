#include "HestiaCommands.h"

#include "ConsoleInterface.h"
#include "StringUtils.h"
#include "User.h"

#include <iostream>
#include <stdexcept>

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
    m_subject.m_system_type = subject == User::get_type() ?
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

HestiaClientCommand::OutputFormat
HestiaClientCommand::output_format_from_string(const std::string& format)
{
    return OutputFormat_enum_string_converter().init().from_string(format);
}

bool HestiaClientCommand::expects_id(OutputFormat format)
{
    return format == OutputFormat::ID || format == OutputFormat::ID_KEY_VALUE;
}

bool HestiaClientCommand::expects_attributes(OutputFormat format)
{
    return format == OutputFormat::KEY_VALUE
           || format == OutputFormat::ID_KEY_VALUE
           || format == OutputFormat::JSON;
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

std::pair<HestiaClientCommand::OutputFormat, CrudAttributes::Format>
HestiaClientCommand::parse_create_update_inputs(
    VecCrudIdentifier& ids,
    CrudAttributes& attributes,
    IConsoleInterface* console) const
{
    CrudAttributes::Format input_format{CrudAttributes::Format::NONE};
    if (!m_input_format.empty()) {
        input_format = CrudAttributes::format_from_string(m_input_format);
    }

    OutputFormat output_format{OutputFormat::ID};
    if (!m_output_format.empty()) {
        output_format = output_format_from_string(m_output_format);
    }

    CrudIdentifier::InputFormat id_format{CrudIdentifier::InputFormat::ID};
    if (!m_id_format.empty()) {
        id_format = CrudIdentifier::input_format_from_string(m_id_format);
    }

    if (!m_id.empty()) {
        std::string ids_combined;
        for (const auto& entry : m_id) {
            ids_combined += entry;
        }
        CrudIdentifier::parse(ids_combined, id_format, ids);
    }

    if (input_format != CrudAttributes::Format::NONE) {
        std::string attribute_buffer;
        attributes.set_format(input_format);
        std::string line;
        if (input_format == CrudAttributes::Format::JSON) {
            while (console->console_read(line)) {
                attribute_buffer += line;
            }
        }
        else {
            while (console->console_read(line)) {
                if (line.empty()) {
                    attribute_buffer += '\n';
                }
                if (StringUtils::has_character(line, ',')) {
                    attribute_buffer += line + '\n';
                }
                else {
                    attribute_buffer += '\n';
                    CrudIdentifier id;
                    id.from_buffer(line, id_format);
                    ids.push_back(id);
                }
            }
        }
        attributes.set_buffer(attribute_buffer, input_format);
    }

    CrudAttributes::Format attribute_output_format{
        CrudAttributes::Format::NONE};
    if (output_format == OutputFormat::JSON) {
        attribute_output_format = CrudAttributes::Format::JSON;
    }
    else if (
        output_format == OutputFormat::KEY_VALUE
        || output_format == OutputFormat::ID_KEY_VALUE) {
        attribute_output_format = CrudAttributes::Format::KEY_VALUE;
    }
    return {output_format, attribute_output_format};
}

std::pair<HestiaClientCommand::OutputFormat, CrudAttributes::Format>
HestiaClientCommand::parse_read_inputs(
    CrudQuery& query, IConsoleInterface* console) const
{
    CrudQuery::Format input_format{CrudQuery::Format::LIST};
    if (!m_input_format.empty()) {
        input_format = CrudQuery::format_from_string(m_input_format);
    }
    else {
        if (!m_body.empty()) {
            input_format = CrudQuery::Format::ID;
        }
    }

    OutputFormat output_format{OutputFormat::ID};
    if (!m_output_format.empty()) {
        output_format = output_format_from_string(m_output_format);
    }
    else {
        if (input_format == CrudQuery::Format::ID) {
            output_format = OutputFormat::JSON;
        }
    }

    CrudQuery::OutputFormat query_output_format{
        CrudQuery::OutputFormat::ATTRIBUTES};
    if (output_format == HestiaClientCommand::OutputFormat::ID) {
        query_output_format = CrudQuery::OutputFormat::ID;
    }
    query.set_output_format(query_output_format);

    CrudAttributes::Format attribute_output_format{
        CrudAttributes::Format::JSON};
    if (output_format == HestiaClientCommand::OutputFormat::KEY_VALUE
        || output_format == HestiaClientCommand::OutputFormat::ID_KEY_VALUE) {
        query.set_attributes_output_format(attribute_output_format);
    }

    CrudIdentifier::InputFormat id_format{CrudIdentifier::InputFormat::ID};
    if (!m_id_format.empty()) {
        id_format = CrudIdentifier::input_format_from_string(m_id_format);
    }
    query.set_format(input_format);

    if (input_format == CrudQuery::Format::ID) {
        VecCrudIdentifier ids;
        if (!m_body.empty()) {
            std::string ids_combined;
            for (const auto& entry : m_body) {
                ids_combined += entry;
            }
            CrudIdentifier::parse(ids_combined, id_format, ids);
        }
        else {
            std::string id_buffer;
            std::string line;
            while (console->console_read(line)) {
                id_buffer += line + '\n';
            }
            CrudIdentifier::parse(id_buffer, id_format, ids);
        }
        query.set_ids(ids);
    }
    else {
        Map filter;
        if (!m_body.empty()) {
            for (const auto& entry : m_body) {
                auto [key, value] = StringUtils::split_on_first(entry, ',');
                StringUtils::trim(key);
                StringUtils::trim(value);
                filter.set_item(key, value);
            }
        }
        query.set_filter(filter);
    }

    return {output_format, attribute_output_format};
}

void HestiaClientCommand::parse_remove_inputs(
    VecCrudIdentifier& ids, IConsoleInterface* console) const
{
    CrudIdentifier::InputFormat id_format{CrudIdentifier::InputFormat::ID};
    if (!m_id_format.empty()) {
        id_format = CrudIdentifier::input_format_from_string(m_id_format);
    }

    if (!m_id.empty()) {
        std::string ids_combined;
        for (const auto& entry : m_id) {
            ids_combined += entry;
        }
        CrudIdentifier::parse(ids_combined, id_format, ids);
    }
    else {
        std::string id_buffer;
        std::string line;
        while (console->console_read(line)) {
            id_buffer += line + '\n';
        }
        CrudIdentifier::parse(id_buffer, id_format, ids);
    }
}

}  // namespace hestia