#pragma once

#include "HttpRequest.h"
#include "Stream.h"

namespace hestia {

/**
 * @brief An abstraction of a HTTP client
 *
 * Base class for HTTP clients - the Curl client is a concrete example.
 */
class HttpClient {
  public:
    virtual ~HttpClient() = default;

    /**
     * Make a sync http request and wait for the response
     * @param request the http request
     * @param stream if present will send or receive the message body
     * @return the http response
     */
    virtual HttpResponse::Ptr make_request(
        const HttpRequest& request, Stream* stream = nullptr) = 0;
};
}  // namespace hestia