#include "TokenAuthenticationMiddleware.h"

#include "Logger.h"

#include <cassert>
#include <stdexcept>

namespace hestia {

HttpResponse::Ptr TokenAuthenticationMiddleware::call(
    const HttpRequest& request,
    AuthorizationContext& auth,
    HttpEvent event,
    responseProviderFunc func)
{
    if (event != HttpEvent::HEADERS) {
        return func(request);
    }

    LOG_INFO("Into TokenAuthenticationMiddleware");
    if (auto auth_token = request.get_header().get_item("Authorization");
        !auth_token.empty()) {
        auto auth_response =
            m_user_service->authenticate_with_token(auth_token);
        if (auth_response->ok()) {
            auth.m_user_id    = auth_response->get_item()->get_primary_key();
            auth.m_user_token = auth_token;
            LOG_INFO("Authenticated user: " + auth.m_user_id);
        }
        else {
            LOG_INFO("User not found.");
        }
    }
    else {
        LOG_INFO("Auth token not specified.");
    }
    return func(request);
}
}  // namespace hestia