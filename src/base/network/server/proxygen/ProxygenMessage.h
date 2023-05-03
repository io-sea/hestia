#pragma once

#include "HttpRequest.h"

#ifdef HAVE_PROXYGEN
namespace proxygen {
class HTTPMessage;
class ResponseHandler;
}  // namespace proxygen

namespace hestia {
class ProxygenMessage {
  public:
    static HttpRequest to_request(proxygen::HTTPMessage* message);

    static void build_response(
        proxygen::ResponseHandler* response_handler,
        const HttpResponse* response,
        bool end_of_message);
};
}  // namespace hestia
#endif