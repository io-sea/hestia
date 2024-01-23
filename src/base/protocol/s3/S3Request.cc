#include "S3Request.h"

#include "XmlAttribute.h"
#include "XmlElement.h"

#include "HashUtils.h"
#include "Logger.h"

#include <algorithm>

namespace hestia {

const char empty_payload_sha[] =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

S3Request::S3Request(const std::string& domain) : m_domain(domain) {}

S3Request::S3Request(
    const S3UserContext& user_context, const std::string& domain) :
    m_user_context(user_context), m_domain(domain)
{
    m_signed_headers = {"host", "x-amz-content-sha256", "x-amz-date"};
}

S3Request::S3Request(
    const HttpRequest& req, const std::string& domain, bool parse_auth)
{
    m_domain      = domain;
    m_path        = S3Path(req, domain);
    m_tracking_id = req.get_tracking_id();

    if (parse_auth) {
        parse_authorisation_info(req);
    }
}

const std::string& S3Request::get_bucket_name() const
{
    return m_path.m_bucket_name;
}

const std::string& S3Request::get_object_key() const
{
    return m_path.m_object_key;
}

void S3Request::parse_authorisation_info(const HttpRequest& request)
{
    const auto auth_item = request.get_header().get_item("Authorization");

    if (auth_item.rfind("AWS4-HMAC-SHA256", 0) != 0) {
        on_error(S3StatusCode::_400_INVALID_SIGNATURE_TYPE);
        LOG_ERROR("Bad signature: " << to_string());
        return;
    }

    const auto credentials = extract_string_part(auth_item, "Credential", ",");
    const auto signed_headers =
        extract_string_part(auth_item, "SignedHeaders", ",");
    const auto signature = extract_string_part(auth_item, "Signature", "");

    if (credentials.empty() || signed_headers.empty() || signature.empty()) {
        on_error(S3StatusCode::_400_AUTHORIZATION_HEADER_MALFORMED);
        LOG_ERROR("Bad auth header: " << to_string());
        return;
    }

    m_signature = signature;

    parse_signed_headers(signed_headers);
    parse_queries(request);
    parse_credentials(credentials);

    m_status = Status::VALID;
    return;
}

void S3Request::parse_signed_headers(const std::string& input_headers)
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

void S3Request::sort_headers()
{
    std::sort(m_signed_headers.begin(), m_signed_headers.end());
}

void S3Request::parse_queries(const HttpRequest& request)
{
    auto on_item = [this](const std::string& key, const std::string& value) {
        add_query({key, value});
    };
    request.get_queries().for_each_item(on_item);
    sort_queries();
}

void S3Request::add_query(const std::pair<std::string, std::string>& query)
{
    m_queries.push_back(query);
}

void S3Request::sort_queries()
{
    std::sort(m_queries.begin(), m_queries.end());
}

void S3Request::parse_credentials(const std::string& input_credentials)
{
    auto creds      = input_credentials;
    auto next_slash = creds.find('/');

    m_user_context.m_user_id = creds.substr(0, next_slash);
    creds                    = creds.substr(next_slash + 1);

    next_slash          = creds.find('/');
    m_timestamp.m_value = creds.substr(0, next_slash);
    creds               = creds.substr(next_slash + 1);

    next_slash = creds.find('/');
    m_region   = creds.substr(0, next_slash);
}

std::string S3Request::extract_string_part(
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

void S3Request::on_error(S3StatusCode error_code)
{
    m_error_code = error_code;
    m_status     = Status::FAILED;
}

void S3Request::add_signed_header(const std::string& header_key)
{
    auto header_cp = header_key;
    std::transform(
        header_cp.begin(), header_cp.end(), header_cp.begin(), ::tolower);
    m_signed_headers.push_back(header_cp);
}

std::string S3Request::get_resource_path(
    const std::string& bucket_name, const std::string& object_name) const
{
    std::string path = m_endpoint;
    if (m_uri_style == S3UriStyle::PATH) {
        path += "/" + bucket_name;
    }
    if (!object_name.empty()) {
        path += "/" + object_name;
    }
    return path;
}

std::string S3Request::get_resource_host(const std::string& bucket_name) const
{
    std::string host = m_endpoint;
    if (m_uri_style == S3UriStyle::VIRTUAL_HOST) {
        host = bucket_name + "." + host;
    }
    return host;
}

void S3Request::add_aws_namespace(XmlElement& element)
{
    auto ns_attr = XmlAttribute::create("xmlns");
    ns_attr->set_value("http://s3.amazonaws.com/doc/2006-03-01/");
    element.add_attribute(std::move(ns_attr));
}

std::string S3Request::to_string() const
{
    std::stringstream sstr;
    sstr << "S3Request:\n";
    sstr << "status = " << to_string(m_status) << '\n';
    sstr << "user_identifier = " << m_user_context.m_user_id << '\n';
    sstr << "user_key = " << m_user_context.m_user_secret_key << '\n';
    sstr << "date = " << m_timestamp.m_value << '\n';
    sstr << "region = " << m_region << '\n';
    sstr << "bucket_name = " << m_path.m_bucket_name << '\n';
    sstr << "object_key = " << m_path.m_object_key << '\n';
    sstr << "signed_headers = ";
    for (const auto& header : m_signed_headers) {
        sstr << header << ';';
    }
    sstr << '\n';
    sstr << "signature = " << m_signature << '\n';
    return sstr.str();
}

std::string S3Request::to_string(Status status)
{
    switch (status) {
        case Status::UNSET:
            return "UNSET";
        case Status::FAILED:
            return "FAILED";
        case Status::VALID:
            return "VALID";
        default:
            return "UKNOWN";
    };
}

void S3Request::populate_headers(
    const std::string& bucket_name,
    HttpHeader& header,
    const std::string& payload_type) const
{
    if (payload_type.empty()) {
        header.set_item("content-type", m_payload_type);
    }
    else {
        header.set_item("content-type", "application/xml");
    }
    header.set_item("x-amz-date", m_timestamp.m_value);
    header.set_item("host", get_resource_host(bucket_name));
    for (const auto& [key, value] : m_extra_headers.data()) {
        header.set_item(key, value);
    }
}

void S3Request::populate_authorization_headers(
    PayloadSignatureType payload_sig_type, HttpRequest& req) const
{
    std::string payload_sig = "UNSIGNED-PAYLOAD";
    if (payload_sig_type == PayloadSignatureType::SIGNED) {
        if (req.body().empty()) {
            payload_sig = empty_payload_sha;
        }
        else {
            payload_sig = HashUtils::do_sha256(req.body());
        }
    }
    req.get_header().set_item("x-amz-content-sha256", payload_sig);

    const auto credential = m_user_context.m_user_id + "/" + get_scope();
    const std::string auth_value =
        "AWS4-HMAC-SHA256 Credential=" + credential
        + ",SignedHeaders=" + get_signed_headers_flat() + ",Signature="
        + get_signature(req, payload_sig, m_user_context.m_user_secret_key);
    req.get_header().set_item("authorization", auth_value);
}

std::string S3Request::get_signed_headers_flat() const
{
    std::stringstream header_sstr;
    for (const auto& header : m_signed_headers) {
        header_sstr << header + ";";
    }
    std::string headers = header_sstr.str();
    if (!headers.empty()) {
        headers = headers.substr(0, headers.size() - 1);
    }
    return headers;
}

std::string S3Request::create_canonical_request(
    const HttpRequest& request, const std::string& calculated_payload) const
{
    std::stringstream sstr;
    sstr << request.get_method_as_string() << '\n';

    S3Path s3_path(request, m_domain);

    std::string canonical_path;
    if (!s3_path.m_is_virtual_host && !s3_path.m_bucket_name.empty()) {
        canonical_path +=
            "/" + HashUtils::uri_encode(s3_path.m_bucket_name, false);
    }
    if (!s3_path.m_object_key.empty()) {
        canonical_path +=
            "/" + HashUtils::uri_encode(s3_path.m_object_key, false);
    }
    if (canonical_path.empty()) {
        canonical_path = "/\n";
    }
    else {
        canonical_path += "\n";
    }
    sstr << canonical_path;
    sstr << serialize_queries();
    sstr << serialize_headers(request);
    sstr << '\n';

    std::string payload_sha256 =
        request.get_header().get_item("x-amz-content-sha256");
    if (payload_sha256 == "UNSIGNED-PAYLOAD") {
        sstr << "UNSIGNED-PAYLOAD";
    }
    else {
        sstr << calculated_payload;
    }
    return sstr.str();
}

std::string S3Request::create_string_to_sign(
    const HttpRequest& request, const std::string& payload_sha256) const
{
    std::stringstream sstr;
    sstr << "AWS4-HMAC-SHA256\n";
    std::string timestamp = request.get_header().get_item("x-amz-date");
    if (timestamp.empty()) {
        return "";
    }
    sstr << timestamp << '\n';
    sstr << get_scope() << '\n';

    const auto canonical = create_canonical_request(request, payload_sha256);
    sstr << HashUtils::do_sha256(canonical);
    return sstr.str();
}

std::string S3Request::get_signature(
    const HttpRequest& req,
    const std::string& payload_sha256,
    const std::string& secret_key) const
{
    return get_signature(
        create_string_to_sign(req, payload_sha256), secret_key);
}

std::string S3Request::get_signature(
    const std::string& string_to_sign, const std::string& secret_key) const
{
    const auto key = "AWS4" + secret_key;
    const auto date_key =
        HashUtils::do_h_mac(key, m_timestamp.m_value.substr(0, 8));
    const auto date_region_key = HashUtils::do_h_mac(date_key, m_region);
    const auto date_region_service_key =
        HashUtils::do_h_mac(date_region_key, m_service);
    const auto signing_key =
        HashUtils::do_h_mac(date_region_service_key, m_scope_suffix);
    return HashUtils::do_h_mac_hex(signing_key, string_to_sign);
}

std::string S3Request::get_scope() const
{
    return m_timestamp.m_value.substr(0, 8) + "/" + m_region + "/" + m_service
           + "/" + m_scope_suffix;
}

std::string S3Request::serialize_headers(const HttpRequest& request) const
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

std::string S3Request::serialize_queries() const
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

}  // namespace hestia