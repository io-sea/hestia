#include "HttpRequest.h"

#include "HttpParser.h"
#include "RequestContext.h"

#include "Logger.h"
#include "StringUtils.h"

#include <iostream>

namespace hestia {

HttpRequest::HttpRequest(
    const std::string& path, Method method, const HttpHeader& header) :
    m_method(method), m_header(header)
{
    m_preamble.m_path = path;
}

void HttpRequest::set_tracking_id(const std::string& id)
{
    m_tracking_id = id;
}

const std::string& HttpRequest::get_tracking_id() const
{
    return m_tracking_id;
}

void HttpRequest::on_chunk(const std::string& msg)
{
    LOG_INFO(msg);
    if (m_has_read_header) {
        m_body += msg;
    }
    else {
        std::stringstream ss(msg);
        std::string buffer;
        bool first_line = true;
        std::size_t body_offset{0};

        while (std::getline(ss, buffer, '\n')) {
            if (!m_has_read_preamble) {
                HttpParser::parse_preamble(buffer, m_preamble);
                m_has_read_preamble = true;

                if (m_preamble.m_method == "GET") {
                    m_method = Method::GET;
                }
                else if (m_preamble.m_method == "PUT") {
                    m_method = Method::PUT;
                }
                else if (m_preamble.m_method == "POST") {
                    m_method = Method::POST;
                }
                else if (m_preamble.m_method == "DELETE") {
                    m_method = Method::DELETE;
                }
                else if (m_preamble.m_method == "HEAD") {
                    m_method = Method::HEAD;
                }
            }
            else if (buffer == "\r") {
                if (first_line) {
                    m_header.add_line(m_header_buffer + buffer);
                    m_header_buffer.clear();
                }
                else {
                    m_has_read_header = true;
                    body_offset       = std::size_t(ss.tellg());
                    break;
                }
            }
            else {
                if (StringUtils::ends_with(buffer, "\r")) {
                    m_header.add_line(m_header_buffer + buffer);
                    m_header_buffer.clear();
                }
                else {
                    m_header_buffer = buffer;
                }
            }
            first_line = false;
        }

        if (body_offset > 0 && body_offset < msg.size()) {
            m_body += msg.substr(body_offset, msg.size() - body_offset);
        }
    }
}

bool HttpRequest::has_read_header() const
{
    return m_has_read_header;
}

const std::string& HttpRequest::body() const
{
    return m_body;
}

HttpHeader& HttpRequest::get_header()
{
    return m_header;
}

const std::string& HttpRequest::get_path() const
{
    return m_preamble.m_path;
}

RequestContext* HttpRequest::get_context() const
{
    return m_context;
}

bool HttpRequest::is_content_outstanding() const
{
    auto content_length = m_header.get_item("Content-Length");
    if (content_length.empty()) {
        return false;
    }

    return std::stoul(content_length) > m_body.size();
}

void HttpRequest::overwrite_path(const std::string& new_path)
{
    m_preamble.m_path = new_path;
}

void HttpRequest::set_context(RequestContext* context)
{
    m_context = context;
}

std::string HttpRequest::get_method_as_string() const
{
    switch (m_method) {
        case Method::GET:
            return "GET";
        case Method::PUT:
            return "PUT";
        case Method::POST:
            return "POST";
        case Method::DELETE:
            return "DELETE";
        case Method::HEAD:
            return "HEAD";
        case Method::UNSET:
            return "UNSET";
        default:
            return "UNKNOWN";
    }
}

const HttpRequest::Method& HttpRequest::get_method() const
{
    return m_method;
}

const HttpHeader& HttpRequest::get_header() const
{
    return m_header;
}

void HttpRequest::set_content_length(std::size_t size)
{
    get_header().set_item("content-length", std::to_string(size));
}

void HttpRequest::set_query(const std::string& key, const std::string& value)
{
    m_preamble.m_queries.set_item(key, value);
}

std::string HttpRequest::to_string() const
{
    std::string ret;
    ret += "Method: " + get_method_as_string() + "\n";
    ret += "Path: " + get_path() + "\n";
    ret += "Header: " + m_header.to_string() + "\n";
    ret += "Body: " + m_body;
    return ret;
}

std::string HttpRequest::get_query_as_string() const 
{
    std::string ret;
    if(m_preamble.m_queries.empty())
    {
        return ret;
    }

    ret += "?";
    for(const auto& [key, value]: m_preamble.m_queries.data()){
        ret += key+"="+value+"&";
    }
    return ret.substr(0, ret.length() -1);
}

}  // namespace hestia