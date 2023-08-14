#include "S3AuthorisationObject.h"

#include "HashUtils.h"
#include "S3Path.h"

#include "Logger.h"

#include <sstream>

namespace hestia {
bool S3AuthorisationObject::is_initialized() const
{
    return m_status != Status::UNSET;
}

bool S3AuthorisationObject::is_valid() const
{
    return m_status == Status::VALID;
}

bool S3AuthorisationObject::is_waiting_for_payload() const
{
    return m_status == Status::WAITING_FOR_PAYLOAD;
}

void S3AuthorisationObject::on_error(const S3Error& error)
{
    m_error  = error;
    m_status = Status::FAILED;
}

void S3AuthorisationObject::on_waiting_for_payload()
{
    m_status = Status::WAITING_FOR_PAYLOAD;
}

void S3AuthorisationObject::set_is_valid()
{
    m_status = Status::VALID;
}

void S3AuthorisationObject::add_chunk(const std::string& chunk)
{
    m_payload += chunk;
}

void S3AuthorisationObject::add_signed_header(const std::string& header)
{
    auto header_cp = header;
    std::transform(
        header_cp.begin(), header_cp.end(), header_cp.begin(), ::tolower);
    m_signed_headers.push_back(header_cp);
}

void S3AuthorisationObject::add_query(
    const std::pair<std::string, std::string>& query)
{
    m_queries.push_back(query);
}

void S3AuthorisationObject::sort_headers()
{
    std::sort(m_signed_headers.begin(), m_signed_headers.end());
}

void S3AuthorisationObject::sort_queries()
{
    std::sort(m_queries.begin(), m_queries.end());
}

std::string S3AuthorisationObject::create_canonical_request(
    const HttpRequest& request) const
{
    std::stringstream sstr;
    sstr << request.get_method_as_string() << '\n';

    S3Path s3_path(request);
    sstr << '/' << HashUtils::uri_encode(s3_path.m_object_id, false) << '\n';
    sstr << serialize_queries();
    sstr << serialize_headers(request);
    sstr << '\n';

    std::string payload_sha256 =
        request.get_header().get_item("x-amz-content-sha256");
    if (payload_sha256 == "UNSIGNED-PAYLOAD") {
        sstr << "UNSIGNED-PAYLOAD";
    }
    else {
        sstr << HashUtils::do_sha256(m_payload);
    }
    return sstr.str();
}

std::string S3AuthorisationObject::create_string_to_sign(
    const HttpRequest& request) const
{
    std::stringstream sstr;
    sstr << "AWS4-HMAC-SHA256\n";
    std::string timestamp = request.get_header().get_item("x-amz-date");
    if (timestamp.empty()) {
        return "";
    }
    sstr << timestamp << '\n';
    sstr << get_scope() << '\n';
    sstr << HashUtils::do_sha256(create_canonical_request(request));
    return sstr.str();
}

std::string S3AuthorisationObject::get_signature(const HttpRequest& req) const
{
    return get_signature(create_string_to_sign(req));
}

std::string S3AuthorisationObject::get_signature(
    const std::string& string_to_sign) const
{
    const auto key             = "AWS4" + m_user_key;
    const auto date_key        = HashUtils::do_h_mac(key, m_date);
    const auto date_region_key = HashUtils::do_h_mac(date_key, m_region);
    const auto date_region_service_key =
        HashUtils::do_h_mac(date_region_key, m_service);
    const auto signing_key =
        HashUtils::do_h_mac(date_region_service_key, m_scope_suffix);
    return HashUtils::do_h_mac_hex(signing_key, string_to_sign);
}

std::string S3AuthorisationObject::get_scope() const
{
    return m_date + "/" + m_region + "/" + m_service + "/" + m_scope_suffix;
}

std::string S3AuthorisationObject::get_credential_header(
    const HttpRequest& req) const
{
    const auto credential = m_user_identifier + "/" + get_scope();

    std::stringstream header_sstr;
    for (const auto& header : m_signed_headers) {
        header_sstr << header + ";";
    }
    std::string headers = header_sstr.str();
    if (!headers.empty()) {
        headers = headers.substr(0, headers.size() - 1);
    }
    return "AWS4-HMAC-SHA256 Credential=" + credential
           + ",SignedHeaders=" + headers + ",Signature=" + get_signature(req);
}

std::string S3AuthorisationObject::serialize_headers(
    const HttpRequest& request) const
{
    std::stringstream sstr;
    for (auto it = m_signed_headers.begin(); it != m_signed_headers.end();
         ++it) {
        std::string header_value = request.get_header().get_item(*it);
        if (header_value.empty()) {
            continue;
        }

        while (isspace(header_value[0]) != 0) {
            header_value.erase(0, 1);
        }
        while (isspace(header_value[header_value.size() - 1]) != 0) {
            header_value.erase(header_value.size() - 2, 1);
        }
        sstr << *it << ':' << header_value << '\n';
    }
    sstr << '\n';
    if (!m_signed_headers.empty()) {
        for (size_t i = 0; i < m_signed_headers.size() - 1; ++i) {
            sstr << m_signed_headers[i] << ';';
        }
        sstr << m_signed_headers[m_signed_headers.size() - 1];
    }
    return sstr.str();
}

std::string S3AuthorisationObject::serialize_queries() const
{
    if (m_queries.empty()) {
        return "\n";
    }
    std::stringstream sstr;
    for (size_t i = 0; i < m_queries.size() - 1; ++i) {
        sstr << m_queries[i].first << '=' << m_queries[i].second << '&';
    }
    sstr << m_queries[m_queries.size() - 1].first << '='
         << m_queries[m_queries.size() - 1].second;
    sstr << '\n';
    return sstr.str();
}

bool S3AuthorisationObject::parse_authorisation_info(const HttpRequest& request)
{
    const auto auth_item = request.get_header().get_item("Authorization");

    if (auth_item.rfind("AWS4-HMAC-SHA256", 0) != 0) {
        on_error(
            {S3Error::Code::_400_INVALID_SIGNATURE_TYPE, request.get_path()});
        LOG_ERROR("Bad signature: " << to_string());
        LOG_ERROR(to_string());
        return false;
    }

    const auto credentials = extract_string_part(auth_item, "Credential", ",");
    const auto signed_headers =
        extract_string_part(auth_item, "SignedHeaders", ",");
    const auto signature = extract_string_part(auth_item, "Signature", "");

    if (credentials.empty() || signed_headers.empty() || signature.empty()) {
        on_error(
            {S3Error::Code::_400_AUTHORIZATION_HEADER_MALFORMED,
             request.get_path()});
        LOG_ERROR("Bad auth header: " << to_string());
        return false;
    }

    m_signature = signature;

    parse_signed_headers(signed_headers);
    parse_queries(request);
    parse_credentials(credentials);
    return true;
}

void S3AuthorisationObject::parse_signed_headers(
    const std::string& input_headers)
{
    if (input_headers.empty()) {
        return;
    }
    size_t index        = 0;
    std::string headers = input_headers;
    while (index != headers.npos) {
        index = headers.find(';');
        add_signed_header(headers.substr(0, index));
        headers = headers.substr(index + 1);
    }
    sort_headers();
}

void S3AuthorisationObject::parse_queries(const HttpRequest& request)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        add_query({key, value});
    };
    request.get_queries().for_each_item(on_item);
    sort_queries();
}

void S3AuthorisationObject::parse_credentials(
    const std::string& input_credentials)
{
    auto creds      = input_credentials;
    auto next_slash = creds.find('/');

    m_user_identifier = creds.substr(0, next_slash);
    creds             = creds.substr(next_slash + 1);

    next_slash = creds.find('/');
    m_date     = creds.substr(0, next_slash);
    creds      = creds.substr(next_slash + 1);

    next_slash = creds.find('/');
    m_region   = creds.substr(0, next_slash);
}

std::string S3AuthorisationObject::to_string() const
{
    std::stringstream sstr;
    sstr << "Authorization object:\n";
    sstr << "status = " << to_string(m_status) << '\n';
    sstr << "user_identifier = " << m_user_identifier << '\n';
    sstr << "user_key = " << m_user_key << '\n';
    sstr << "date = " << m_date << '\n';
    sstr << "region = " << m_region << '\n';
    sstr << "signed_headers = ";
    for (const auto& header : m_signed_headers) {
        sstr << header << ';';
    }
    sstr << '\n';
    sstr << "signature = " << m_signature << '\n';
    sstr << "payload = " << m_payload << '\n';
    return sstr.str();
}

std::string S3AuthorisationObject::extract_string_part(
    const std::string& input,
    const std::string& identifier,
    const std::string& delimiter)
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

std::string S3AuthorisationObject::to_string(Status status)
{
    switch (status) {
        case Status::UNSET:
            return "UNSET";
        case Status::WAITING_FOR_PAYLOAD:
            return "WAITING_FOR_PAYLOAD";
        case Status::FAILED:
            return "FAILED";
        case Status::VALID:
            return "VALID";
        default:
            return "UKNOWN";
    };
}
}  // namespace hestia