#pragma once

#include "HttpRequest.h"

namespace hestia {

/**
 * @brief An abstraction of a HTTP client
 *
 * Base class for HTTP clients - the Curl client is a concrete example.
 */
class HttpClient {
  public:
    ~HttpClient() = default;

    /**
     * Make a sync http request and wait for the response
     * @param request the http request
     * @return the http response
     */
    virtual HttpResponse::Ptr make_request(const HttpRequest& request) = 0;
};
}  // namespace hestia