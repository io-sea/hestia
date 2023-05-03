#include "S3AuthorisationObject.h"

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

void S3AuthorisationObject::add_query(const std::string& query)
{
    size_t loc = query.find('=');
    if (loc != query.npos) {
        m_queries.push_back(std::make_pair<std::string, std::string>(
            query.substr(0, loc), query.substr(loc + 1)));
    }
    else {
        m_queries.push_back(
            std::make_pair<std::string, std::string>(query.c_str(), ""));
    }
}

void S3AuthorisationObject::sort_headers()
{
    std::sort(m_signed_headers.begin(), m_signed_headers.end());
}

void S3AuthorisationObject::sort_queries()
{
    std::sort(m_queries.begin(), m_queries.end());
}

std::string S3AuthorisationObject::serialize_headers(
    const HttpRequest& request) const
{
    std::stringstream sstr;
    for (auto it = m_signed_headers.begin(); it != m_signed_headers.end();
         ++it) {
        std::string header_value = request.get_header().get_item(*it);
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