#include "HestiaRequest.h"

namespace hestia {

HestiaRequest::HestiaRequest(HestiaType subject, CrudMethod method) :
    m_subject(subject), m_type(Type::CRUD), m_crud_request(method)
{
}

HestiaRequest::HestiaRequest(HestiaType subject, const HsmAction& action) :
    m_subject(subject), m_type(Type::HSM_ACTION), m_action(action)
{
}

bool HestiaRequest::is_crud_request() const
{
    return m_type == Type::CRUD;
}

bool HestiaRequest::is_hsm_action() const
{
    return m_type == Type::HSM_ACTION;
}

const CrudQuery::OutputFormat& HestiaRequest::get_output_format() const
{
    return m_crud_request.get_query().get_output_format();
}

bool HestiaRequest::is_create_method() const
{
    return is_crud_request() && m_crud_request.is_create_method();
}

bool HestiaRequest::is_read_method() const
{
    return is_crud_request() && m_crud_request.is_read_method();
}

bool HestiaRequest::is_identify_method() const
{
    return is_crud_request() && m_crud_request.is_identify_method();
}

bool HestiaRequest::is_update_method() const
{
    return is_crud_request() && m_crud_request.is_update_method();
}

bool HestiaRequest::is_remove_method() const
{
    return is_crud_request() && m_crud_request.is_remove_method();
}

bool HestiaRequest::is_create_or_update_method() const
{
    return is_create_method() || is_update_method();
}

HsmAction& HestiaRequest::action()
{
    return m_action;
}

const HsmAction& HestiaRequest::get_action() const
{
    return m_action;
}

const CrudRequest& HestiaRequest::get_crud_request() const
{
    return m_crud_request;
}

bool HestiaRequest::supports_stream_source() const
{
    return m_action.is_data_put_action();
}

bool HestiaRequest::supports_stream_sink() const
{
    return m_action.is_data_io_action() && !supports_stream_source();
}

bool HestiaRequest::is_system_type() const
{
    return m_subject.m_type == HestiaType::Type::SYSTEM;
}

bool HestiaRequest::is_hsm_type() const
{
    return m_subject.m_type == HestiaType::Type::HSM;
}

bool HestiaRequest::is_user_type() const
{
    return is_system_type()
           && m_subject.m_system_type == HestiaType::SystemType::USER;
}

void HestiaRequest::set_ids(const CrudIdentifierCollection& ids)
{
    m_crud_request.set_ids(ids);
}

void HestiaRequest::append(
    const std::string& body, CrudAttributes::FormatSpec format)
{
    m_crud_request.append(body, format);
}

void HestiaRequest::set_offset_and_count(std::size_t offset, std::size_t count)
{
    m_crud_request.set_offset_and_count(offset, count);
}

std::string HestiaRequest::get_hsm_type_as_string() const
{
    return HsmItem::to_name(m_subject.m_hsm_type);
}

void HestiaRequest::set_output_format(const CrudQuery::OutputFormat& format)
{
    m_crud_request.set_output_format(format);
}

void HestiaRequest::set_query_filter(const Map& filter)
{
    m_crud_request.set_query_filter(filter);
}

}  // namespace hestia