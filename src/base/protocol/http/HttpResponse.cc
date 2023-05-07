#include "HttpResponse.h"

namespace hestia {
HttpResponse::HttpResponse(
    int code, const std::string& message, const std::string& body) :
    m_code(code), m_message(message), m_body(body)
{
}

HttpResponse::Ptr HttpResponse::create()
{
    return std::make_unique<HttpResponse>();
}

HttpResponse::Ptr HttpResponse::create(
    int code, const std::string& message, const std::string& body)
{
    return std::make_unique<HttpResponse>(code, message, body);
}

HttpHeader& HttpResponse::header()
{
    return m_header;
}

const HttpHeader& HttpResponse::header() const
{
    return m_header;
}

int HttpResponse::code() const
{
    return m_code;
}

const std::string& HttpResponse::message() const
{
    return m_message;
}

const std::string& HttpResponse::body() const
{
    return m_body;
}

void HttpResponse::set_body(const std::string& body)
{
    m_body = body;
}

void HttpResponse::append_to_body(const std::string& body)
{
    m_body += body;
}

std::string HttpResponse::to_string() const
{
    std::stringstream sstr;
    sstr << "HTTP/1.1"
         << " " << m_code << " " << m_message << "\n";
    sstr << m_header.to_string();
    sstr << "\n";
    sstr << m_body;
    return sstr.str();
}

}  // namespace hestia