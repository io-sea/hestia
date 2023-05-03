#pragma once

#include "HttpHeader.h"

#include <memory>
#include <sstream>

namespace hestia {
class HttpResponse {
  public:
    using Ptr = std::unique_ptr<HttpResponse>;

    HttpResponse() = default;
    HttpResponse(
        int code, const std::string& message, const std::string& body = {});

    static Ptr create();
    static Ptr create(
        int code, const std::string& message, const std::string& body = {});

    bool error() const { return m_code >= 400; }

    int code() const;

    const std::string& message() const;

    const std::string& body() const;

    HttpHeader& header();

    const HttpHeader& header() const;

    void append_to_body(const std::string& body);

    void set_body(const std::string& body);

    std::string to_string() const;

  private:
    int m_code{200};
    std::string m_message{"OK"};
    std::string m_body;
    HttpHeader m_header;
};
}  // namespace hestia