#pragma once

#include "CurlHandle.h"
#include "HttpClient.h"

#include <atomic>
#include <thread>
#include <unordered_map>

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
    void make_request(
        const HttpRequest& request,
        completionFunc completion_func,
        Stream* stream                = nullptr,
        std::size_t progress_interval = 0,
        progressFunc progess_func     = nullptr) override;

  private:
    static size_t curl_write_data(
        void* buffer, size_t size, size_t nmemb, void* userp);

    size_t on_write(void* buffer, size_t nmemb);

    static size_t curl_read_data(
        char* buffer, size_t size, size_t nmemb, void* userp);

    size_t on_read(char* buffer, size_t nmemb);

    static size_t curl_seek_data(void* userp, curl_off_t offset, int origin);

    size_t on_seek(curl_off_t offset, int origin);

    void setup_handle(CurlHandle* handle);

    CurlClientConfig m_config;
    std::atomic<bool> m_initialized{false};

    std::unordered_map<std::thread::id, CurlHandle*> m_handles;
};
}  // namespace hestia