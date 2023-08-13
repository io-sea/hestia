#pragma once

#include "HttpPreamble.h"
#include "HttpResponse.h"

namespace hestia {
class RequestContext;

class HttpRequest {
  public:
    enum class Method { GET, PUT, POST, DELETE, HEAD, UNSET };

    HttpRequest() = default;

    HttpRequest(
        const std::string& path, Method method, const HttpHeader& header = {});

    const std::string& body() const;

    std::string& body() { return m_body; }

    HttpHeader& get_header();

    const std::string& get_path() const;

    std::string get_method_as_string() const;

    const Method& get_method() const;

    const Map& get_queries() const { return m_preamble.m_queries; }

    void overwrite_path(const std::string& new_path);

    void set_queries(const Map& queries) { m_preamble.m_queries = queries; }

    const HttpHeader& get_header() const;

    RequestContext* get_context() const;

    void on_chunk(const std::string& msg);

    bool has_read_header() const;

    bool is_content_outstanding() const;

    std::string to_string() const;

    void set_context(RequestContext* context);

  private:
    RequestContext* m_context{nullptr};
    Method m_method{Method::UNSET};

    std::string m_body;
    HttpPreamble m_preamble;
    HttpHeader m_header;
    bool m_has_read_header{false};
    bool m_has_read_preamble{false};
};

using responseProviderFunc =
    std::function<HttpResponse::Ptr(const HttpRequest&)>;
}  // namespace hestia