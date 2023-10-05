#include "S3AuthorisationSession.h"

#include "HashUtils.h"
#include "Logger.h"
#include "S3Path.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace hestia {
S3AuthorisationSession::S3AuthorisationSession(UserService* service) :
    m_user_service(service)
{
}

bool S3AuthorisationSession::is_valid() const
{
    return m_object.is_valid();
}

void S3AuthorisationSession::add_chunk(const std::string& chunk)
{
    m_object.add_chunk(chunk);
}

const S3AuthorisationObject& S3AuthorisationSession::authorise(
    const HttpRequest& request)
{
    if (!m_object.is_initialized()) {
        if (!m_object.parse_authorisation_info(request)) {
            LOG_ERROR("Failed to parse auth info");
            return m_object;
        }

        if (!search_for_user(request)) {
            LOG_ERROR("Failed to find user");
            return m_object;
        }

        if (has_signed_payload(request)) {
            m_object.on_waiting_for_payload();
        }
        else {
            check(request);
        }
    }
    else if (m_object.is_waiting_for_payload()) {
        check(request);
    }
    return m_object;
}

bool S3AuthorisationSession::search_for_user(const HttpRequest& request)
{
    CrudQuery query(
        KeyValuePair{"name", m_object.m_user_identifier},
        CrudQuery::Format::GET, CrudQuery::OutputFormat::ITEM);
    auto response = m_user_service->make_request(CrudRequest{query, {}});

    if (!response->ok()) {
        m_object.on_error(
            {S3Error::Code::_403_ACCESS_DENIED, S3Request(request)});
        LOG_ERROR(m_object.to_string())
        return false;
    }

    if (!response->found()) {
        m_object.on_error(
            {S3Error::Code::_403_INVALID_KEY_ID, S3Request(request)});
        LOG_ERROR(m_object.to_string())
        return false;
    }

    const auto user_item        = response->get_item_as<User>();
    m_object.m_user_internal_id = user_item->get_primary_key();
    if (!user_item->tokens().empty()) {
        m_object.m_user_key = user_item->get_first_token().value();
    }
    return true;
}

bool S3AuthorisationSession::has_signed_payload(
    const HttpRequest& request) const
{
    const auto content_length = request.get_header().get_item("Content-Length");
    const auto payload_sha256 =
        request.get_header().get_item("x-amz-content-sha256");
    if (content_length.empty() or content_length == "0") {
        return false;
    }
    if (payload_sha256.empty() || payload_sha256 == "UNSIGNED-PAYLOAD") {
        return false;
    }
    return true;
}

void S3AuthorisationSession::check(const HttpRequest& request)
{
    const auto signature = m_object.get_signature(request);
    if (m_object.m_signature == signature) {
        m_object.set_is_valid();
    }
    else {
        m_object.on_error(
            {S3Error::Code::_403_ACCESS_DENIED, S3Request(request)});
        LOG_ERROR(
            "Signatures do not match: " << m_object.m_signature << " | "
                                        << signature << "\n"
                                        << m_object.to_string());
    }
}

}  // namespace hestia
