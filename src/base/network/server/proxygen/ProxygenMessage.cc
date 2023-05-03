#include "ProxygenMessage.h"

#ifdef HAVE_PROXYGEN
#include <proxygen/httpserver/ResponseBuilder.h>

namespace hestia {
HttpRequest ProxygenMessage::to_request(proxygen::HTTPMessage* message)
{
    auto proxygen_method = message->getMethod();

    HttpRequest::Method method{HttpRequest::Method::UNSET};
    if (proxygen_method) {
        switch (proxygen_method.value()) {
            case proxygen::HTTPMethod::GET:
                method = HttpRequest::Method::GET;
                break;
            case proxygen::HTTPMethod::PUT:
                method = HttpRequest::Method::PUT;
                break;
            case proxygen::HTTPMethod::DELETE:
                method = HttpRequest::Method::DELETE;
                break;
            case proxygen::HTTPMethod::HEAD:
                method = HttpRequest::Method::HEAD;
                break;
            default:
                break;
        }
    }

    HttpRequest request(message->getPath(), method);

    auto each_header =
        [&request](const std::string& tag, const std::string& val) {
            request.get_header().set_item(tag, val);
        };
    message->getHeaders().forEach(each_header);
    return request;
}

void ProxygenMessage::build_response(
    proxygen::ResponseHandler* response_handler,
    const HttpResponse* response,
    bool end_of_message)
{
    proxygen::ResponseBuilder response_builder(response_handler);
    response_builder.status(response->code(), response->message());
    response_builder.body(response->body());

    auto each_header =
        [&response_builder](const std::string& tag, const std::string& val) {
            response_builder.header(tag, val);
        };
    response->header().for_each(each_header);

    if (end_of_message) {
        response_builder.sendWithEOM();
    }
    else {
        response_builder.send();
    }
}
}  // namespace hestia
#endif