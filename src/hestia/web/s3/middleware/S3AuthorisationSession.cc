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
        if (!parse_authorisation_info(request)) {
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

bool S3AuthorisationSession::parse_authorisation_info(
    const HttpRequest& request)
{
    const auto auth_item = request.get_header().get_item("Authorization");

    if (auth_item.rfind("AWS4-HMAC-SHA256", 0) != 0) {
        m_object.on_error(
            {S3Error::Code::_400_INVALID_SIGNATURE_TYPE, request.get_path()});
        LOG_ERROR(m_object.to_string());
        return false;
    }

    const auto credentials = extract_string_part(auth_item, "Credential", ",");
    const auto signed_headers =
        extract_string_part(auth_item, "SignedHeaders", ",");
    const auto signature = extract_string_part(auth_item, "Signature", "");

    if (credentials.empty() || signed_headers.empty() || signature.empty()) {
        m_object.on_error(
            {S3Error::Code::_400_AUTHORIZATION_HEADER_MALFORMED,
             request.get_path()});
        LOG_ERROR(m_object.to_string());
        return false;
    }

    m_object.m_signature = signature;

    parse_signed_headers(signed_headers);
    parse_queries(request);
    parse_credentials(credentials);
    return true;
}

void S3AuthorisationSession::parse_signed_headers(
    const std::string& input_headers)
{
    if (input_headers.empty()) {
        return;
    }
    size_t index        = 0;
    std::string headers = input_headers;
    while (index != headers.npos) {
        index = headers.find(';');
        m_object.add_signed_header(headers.substr(0, index));
        headers = headers.substr(index + 1);
    }
    m_object.sort_headers();
}

void S3AuthorisationSession::parse_queries(const HttpRequest& request)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        m_object.add_query({key, value});
    };
    request.get_queries().for_each_item(on_item);
    m_object.sort_queries();
}

void S3AuthorisationSession::parse_credentials(
    const std::string& input_credentials)
{
    auto creds      = input_credentials;
    auto next_slash = creds.find('/');

    m_object.m_user_identifier = creds.substr(0, next_slash);
    creds                      = creds.substr(next_slash + 1);

    next_slash      = creds.find('/');
    m_object.m_date = creds.substr(0, next_slash);
    creds           = creds.substr(next_slash + 1);

    next_slash        = creds.find('/');
    m_object.m_region = creds.substr(0, next_slash);
}

std::string S3AuthorisationSession::extract_string_part(
    const std::string& input,
    const std::string& identifier,
    const std::string& delimiter) const
{
    size_t index = input.find(identifier);
    if (index == input.npos) {
        return "";
    }

    index                  = input.find('=', index) + 1;
    size_t delimiter_index = input.npos;
    if (!delimiter.empty()) {
        delimiter_index = input.find(delimiter, index);
    }
    return input.substr(index, delimiter_index - index);
}

bool S3AuthorisationSession::search_for_user(const HttpRequest& request)
{
    User user;
    user.set_name(m_object.m_user_identifier);

    CrudQuery query(
        KeyValuePair{"name", m_object.m_user_identifier},
        CrudQuery::Format::GET, CrudQuery::OutputFormat::ITEM);
    auto response = m_user_service->make_request(CrudRequest{query, {}});

    if (!response->ok()) {
        m_object.on_error(
            {S3Error::Code::_403_ACCESS_DENIED, request.get_path()});
        LOG_ERROR(m_object.to_string())
        return false;
    }

    if (!response->found()) {
        m_object.on_error(
            {S3Error::Code::_403_INVALID_KEY_ID, request.get_path()});
        LOG_ERROR(m_object.to_string())
        return false;
    }

    const auto user_item = response->get_item_as<User>();
    if (!user_item->tokens().empty()) {
        m_object.m_user_key = user_item->tokens()[0].value();
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
    const auto canonical_request = create_canonical_request(request);
    const auto string_to_sign =
        create_string_to_sign(request, canonical_request);
    const auto signature = get_signature(string_to_sign);
    if (m_object.m_signature == signature) {
        m_object.set_is_valid();
    }
    else {
        m_object.on_error(
            {S3Error::Code::_403_ACCESS_DENIED, request.get_path()});
        LOG_ERROR(
            "Signatures do not match: " << m_object.m_signature << " | "
                                        << signature << "\n"
                                        << m_object.to_string());
    }
}

std::string S3AuthorisationSession::create_canonical_request(
    const HttpRequest& request) const
{
    std::stringstream sstr;
    sstr << request.get_method_as_string() << '\n';

    S3Path s3_path(request);
    sstr << '/' << HashUtils::uri_encode(s3_path.m_object_id, false) << '\n';
    sstr << m_object.serialize_queries();
    sstr << m_object.serialize_headers(request);
    sstr << '\n';

    std::string payload_sha256 =
        request.get_header().get_item("x-amz-content-sha256");
    if (payload_sha256 == "UNSIGNED-PAYLOAD") {
        sstr << "UNSIGNED-PAYLOAD";
    }
    else {
        sstr << HashUtils::do_sha256(m_object.m_payload);
    }
    return sstr.str();
}

std::string S3AuthorisationSession::create_string_to_sign(
    const HttpRequest& request, const std::string& canonical_request) const
{
    std::stringstream sstr;
    sstr << "AWS4-HMAC-SHA256\n";
    std::string timestamp = request.get_header().get_item("x-amz-date");
    if (timestamp.empty()) {
        return "";
    }
    sstr << timestamp << '\n';
    sstr << m_object.m_date << '/' << m_object.m_region << "/s3/aws4_request\n";
    sstr << HashUtils::do_sha256(canonical_request);
    return sstr.str();
}

std::string S3AuthorisationSession::get_signature(
    const std::string& input) const
{
    const auto key            = "AWS4" + m_object.m_user_key;
    const std::string service = "s3";
    const std::string request = "aws4_request";

    const auto date_key = HashUtils::do_h_mac(key, m_object.m_date);
    const auto date_region_key =
        HashUtils::do_h_mac(date_key, m_object.m_region);
    const auto date_region_service_key =
        HashUtils::do_h_mac(date_region_key, service);
    const auto signing_key =
        HashUtils::do_h_mac(date_region_service_key, request);

    return HashUtils::do_h_mac_hex(signing_key, input);
}
}  // namespace hestia
