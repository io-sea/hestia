#pragma once

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Stream.h"

#include <curl/curl.h>

namespace hestia {
struct CurlRequestContext {
    CurlRequestContext() = default;

    CurlRequestContext(
        const HttpRequest* request, HttpResponse* response, Stream* stream) :
        m_request(request), m_response(response), m_stream(stream)
    {
    }

    const HttpRequest* m_request{nullptr};
    HttpResponse* m_response{nullptr};
    Stream* m_stream{nullptr};
    std::size_t m_read_offset{0};
};

class CurlHandle {
  public:
    CurlHandle();

    ~CurlHandle();

    void free();

    void prepare_put(const HttpRequest& request, Stream* stream);

    void prepare_get();

    void prepare_delete();

    void prepare_headers(const HttpHeader& headers);

    void setup();

    CURL* m_handle{nullptr};
    curl_slist* m_headers{nullptr};

    std::string m_error_buffer;
    CurlRequestContext m_request_context;
};
}  // namespace hestia