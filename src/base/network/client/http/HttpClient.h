#pragma once

#include "HttpRequest.h"
#include "Stream.h"

#include <functional>
#include <memory>

namespace hestia {

/**
 * @brief An abstraction of a HTTP client
 *
 * Base class for HTTP clients - the Curl client is a concrete example.
 */
class HttpClient {
  public:
    virtual ~HttpClient() = default;

    using Ptr = std::unique_ptr<HttpClient>;

    /**
     * Make a http request
     * @param request the http request
     * @param stream if present will send or receive the message body
     * @return the http response
     */
    using completionFunc = std::function<void(HttpResponse::Ptr response)>;
    using progressFunc   = std::function<void(std::size_t bytes_transferred)>;
    virtual void make_request(
        const HttpRequest& request,
        completionFunc completion_func,
        Stream* stream                = nullptr,
        std::size_t progress_interval = 0,
        progressFunc progess_func     = nullptr) = 0;
};
}  // namespace hestia