#pragma once

#include "HttpClient.h"

#include <curl/curl.h>

namespace hestia {
struct CurlClientConfig {
    bool m_do_global_init{true};
};

/**
 * @brief A http client using CURL
 *
 */
class CurlClient : public HttpClient {
  public:
    CurlClient(const CurlClientConfig& config);

    virtual ~CurlClient();

    /**
     * Initialize CURL
     */
    void initialize();

    /**
     * Make a sync http request and wait for the response
     * @param request the http request
     * @return the http response
     */
    HttpResponse::Ptr make_request(const HttpRequest& request) override;

  private:
    static size_t curl_write_data(
        void* buffer, size_t size, size_t nmemb, void* userp);
    size_t on_write(void* buffer, size_t nmemb);

    CurlClientConfig m_config;
    HttpResponse::Ptr m_working_response;

    std::string m_error_buffer;
    CURL* m_handle{nullptr};
};
}  // namespace hestia