#include "HestiaCommands.h"

#include "ConsoleInterface.h"
#include "ErrorUtils.h"
#include "HestiaRequest.h"
#include "Logger.h"
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

void HestiaClientCommand::set_subject(HestiaType subject)
{
    m_subject = subject;
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

CrudMethod HestiaClientCommand::get_crud_method() const
{
    return m_crud_method;
}

bool HestiaClientCommand::is_crud_method() const
{
    return m_action.is_crud_method();
}

std::vector<std::string> HestiaClientCommand::get_system_subjects() const
{
    return {"user", "hsm_node"};
}

void HestiaClientCommand::parse_console_inputs(
    const IConsoleInterface& console, HestiaRequest& req) const
{
    if (req.is_create_or_update_method()) {
        parse_create_update_inputs(console, req);
    }
    else if (req.is_read_method()) {
        parse_read_inputs(console, req);
    }
    else if (req.is_remove_method()) {
        parse_remove_inputs(console, req);
    }
}

HestiaClientCommand::BodyFormat HestiaClientCommand::read_input_format(
    BodyFormat default_format) const
{
    BodyFormat_enum_string_converter format_converter;
    format_converter.init();
    auto input_format = default_format;
    if (!m_input_format.empty()) {
        try {
            input_format = format_converter.from_string(m_input_format);
        }
        catch (const std::exception& e) {
            std::string msg =
                SOURCE_LOC()
                + " | Error parsing input_fmt field: " + m_input_format + " - ";
            msg = e.what();
            msg += ". Should be one of: ["
                   + StringUtils::flatten(format_converter.m_values) + "]";
            LOG_ERROR(msg);
            throw std::runtime_error(msg);
        }
    }
    return input_format;
}

CrudQuery::BodyFormat HestiaClientCommand::read_output_format(
    BodyFormat default_format) const
{
    BodyFormat_enum_string_converter format_converter;
    format_converter.init();
    auto output_format = default_format;
    if (!m_output_format.empty()) {
        try {
            output_format = format_converter.from_string(m_output_format);
        }
        catch (const std::exception& e) {
            std::string msg = SOURCE_LOC()
                              + " | Error parsing output_fmt field: "
                              + m_output_format + " - ";
            msg = e.what();
            msg += ". Should be one of: ["
                   + StringUtils::flatten(format_converter.m_values) + "]";
            LOG_ERROR(msg);
            throw std::runtime_error(msg);
        }
    }

    CrudQuery::BodyFormat crud_output_format{CrudQuery::BodyFormat::NONE};
    switch (output_format) {
        case BodyFormat::ID:
            crud_output_format = CrudQuery::BodyFormat::ID;
            break;
        case BodyFormat::KEY_VALUE:
            crud_output_format = CrudQuery::BodyFormat::DICT;
            break;
        case BodyFormat::JSON:
            crud_output_format = CrudQuery::BodyFormat::JSON;
            break;
        case BodyFormat::NONE:
        default:
            crud_output_format = CrudQuery::BodyFormat::NONE;
            break;
    }
    return crud_output_format;
}

CrudIdentifier::InputFormat HestiaClientCommand::read_id_format(
    CrudIdentifier::InputFormat default_format) const
{
    auto id_format = default_format;
    if (!m_id_format.empty()) {
        id_format = CrudIdentifier::input_format_from_string(m_id_format);
    }
    return id_format;
}

void HestiaClientCommand::read_ids_from_console(
    const IConsoleInterface& console,
    HestiaRequest& req,
    CrudIdentifier::InputFormat default_format) const
{
    std::string body;
    console.console_read(body);

    CrudIdentifierCollection::FormatSpec format;
    format.m_id_spec.m_input_format = read_id_format(default_format);

    req.set_ids({body, format});
}

void HestiaClientCommand::parse_create_update_inputs(
    const IConsoleInterface& console, HestiaRequest& req) const
{
    req.set_ids(CrudIdentifierCollection(
        m_id, read_id_format(CrudIdentifier::InputFormat::ID)));

    if (const auto input_format = read_input_format(BodyFormat::NONE);
        input_format == BodyFormat::ID) {
        read_ids_from_console(console, req, CrudIdentifier::InputFormat::ID);
    }
    else if (input_format != BodyFormat::NONE) {
        std::string body;
        console.console_read(body);
        CrudAttributes::FormatSpec format;
        if (input_format == BodyFormat::JSON) {
            format.set_is_json();
        }
        else if (input_format == BodyFormat::KEY_VALUE) {
            format.set_is_dict();
        }
        req.append(body, format);
    }
    req.set_output_format(read_output_format(BodyFormat::ID));
}

void HestiaClientCommand::parse_read_inputs(
    const IConsoleInterface& console, HestiaRequest& req) const
{
    CrudQuery::Format input_format{CrudQuery::Format::LIST};
    if (!m_input_format.empty()) {
        input_format = CrudQuery::format_from_string(m_input_format);
    }
    else if (!m_body.empty()) {
        input_format = CrudQuery::Format::ID;
    }

    if (input_format == CrudQuery::Format::ID) {
        if (!m_body.empty()) {
            req.set_ids(CrudIdentifierCollection(
                m_body, read_id_format(CrudIdentifier::InputFormat::ID)));
        }
        else {
            read_ids_from_console(
                console, req, CrudIdentifier::InputFormat::ID);
        }
    }
    else {
        Map::FormatSpec format;
        req.set_query_filter(Map(m_body, format));
    }

    const auto default_output_format = input_format == CrudQuery::Format::ID ?
                                           BodyFormat::JSON :
                                           BodyFormat::ID;
    req.set_output_format(read_output_format(default_output_format));
}

void HestiaClientCommand::parse_remove_inputs(
    const IConsoleInterface& console, HestiaRequest& req) const
{
    if (!m_id.empty()) {
        req.set_ids(CrudIdentifierCollection(
            m_id, read_id_format(CrudIdentifier::InputFormat::ID)));
    }
    else {
        read_ids_from_console(console, req, CrudIdentifier::InputFormat::ID);
    }
}

void HestiaClientCommand::parse_action(HestiaRequest& req)
{
    req.action().set_subject(m_subject.m_hsm_type);
    req.action().set_target_tier(m_target_tier);
    req.action().set_source_tier(m_source_tier);
    if (!m_id.empty()) {
        req.action().set_subject_key(m_id[0]);
    }
}

}  // namespace hestia