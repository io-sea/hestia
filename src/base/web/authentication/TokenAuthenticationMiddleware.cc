#include "TokenAuthenticationMiddleware.h"

namespace hestia {

HttpResponse::Ptr TokenAuthenticationMiddleware::call(
    const HttpRequest& request, User& user, responseProviderFunc func)
{
    if (auto auth_token = request.get_header().get_item("Authorization");
        !auth_token.empty()) {
        auto auth_response =
            m_user_service->authenticate_with_token(auth_token);
        if (auth_response->ok()) {
            user = auth_response->item();
        }
    }
    return func(request);
}
}  // namespace hestia