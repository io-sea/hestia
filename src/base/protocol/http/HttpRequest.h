#pragma once

#include "HttpPreamble.h"
#include "HttpResponse.h"

namespace hestia {
class RequestContext;

class HttpRequest {
  public:
    enum class Method { GET, PUT, DELETE, HEAD, UNSET };

    HttpRequest() = default;

    HttpRequest(
        const std::string& path, Method method, const HttpHeader& header = {});

    HttpRequest(const std::string& message);

    const std::string& body() const;

    std::string& body() { return m_body; }

    HttpHeader& get_header();

    const std::string& get_path() const;

    std::string get_method_as_string() const;

    const Method& get_method() const;

    const Metadata& get_queries() const { return m_preamble.m_queries; }

    void set_queries(const Metadata& queries)
    {
        m_preamble.m_queries = queries;
    }

    const HttpHeader& get_header() const;

    RequestContext* get_context() const;

    bool is_content_outstanding() const;

    std::string to_string() const;

    void set_context(RequestContext* context);

  private:
    RequestContext* m_context{nullptr};
    Method m_method{Method::UNSET};

    std::string m_body;
    HttpPreamble m_preamble;
    HttpHeader m_header;
};

using responseProviderFunc =
    std::function<HttpResponse::Ptr(const HttpRequest&)>;
}  // namespace hestia