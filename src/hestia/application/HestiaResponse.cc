#include "HestiaResponse.h"

namespace hestia {

HestiaResponse::HestiaResponse(CrudResponse::Ptr crud_response) :
    m_crud_response(std::move(crud_response))
{
    if (!m_crud_response->ok()) {
        m_status = OpStatus(
            OpStatus::Status::ERROR, m_crud_response->get_error().number(),
            m_crud_response->get_error().message());
    }
}

HestiaResponse::HestiaResponse(HsmActionResponse::Ptr action_response) :
    m_action_response(std::move(action_response))
{
    if (!m_action_response->ok()) {
        m_status = OpStatus(
            OpStatus::Status::ERROR, m_action_response->get_error().number(),
            m_action_response->get_error().message());
    }
}

HestiaResponse::HestiaResponse(OpStatus status) : m_status(status) {}

HestiaResponse::Ptr HestiaResponse::create(CrudResponse::Ptr crud_response)
{
    return std::make_unique<HestiaResponse>(std::move(crud_response));
}

HestiaResponse::Ptr HestiaResponse::create(
    HsmActionResponse::Ptr action_response)
{
    return std::make_unique<HestiaResponse>(std::move(action_response));
}

HestiaResponse::Ptr HestiaResponse::create(OpStatus status)
{
    return std::make_unique<HestiaResponse>(status);
}

std::string HestiaResponse::get_action_id() const
{
    return m_action_response->get_action().get_primary_key();
}

std::string HestiaResponse::get_attributes_as_string() const
{
    return m_crud_response->get_attributes().to_string();
}

const CrudIdentifierCollection& HestiaResponse::get_ids() const
{
    return m_crud_response->get_ids();
}

const OpStatus& HestiaResponse::get_status() const
{
    return m_status;
}

bool HestiaResponse::has_ids() const
{
    return !m_crud_response->get_ids().empty();
}

bool HestiaResponse::has_attributes() const
{
    return m_crud_response->expects_attributes();
}

bool HestiaResponse::has_content() const
{
    return has_ids() || has_attributes();
}

bool HestiaResponse::ok() const
{
    return m_status.ok();
}

bool HestiaResponse::is_crud_response() const
{
    return bool(m_crud_response);
}

std::string HestiaResponse::write(const CrudQuery::FormatSpec& format) const
{
    if (is_crud_response()) {
        std::string ret;
        m_crud_response->write(ret, format);
        return ret;
    }
    else {
        return get_action_id();
    }
    return {};
}


}  // namespace hestia