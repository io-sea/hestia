#include "TokenAuthenticationMiddleware.h"

#include <cassert>
#include <stdexcept>

namespace hestia {

HttpResponse::Ptr TokenAuthenticationMiddleware::call(
    const HttpRequest& request, User& user, responseProviderFunc func)
{
    if (auto auth_token = request.get_header().get_item("Authorization");
        !auth_token.empty()) {
        auto auth_response =
            m_user_service->authenticate_with_token(auth_token);
        if (auth_response->ok()) {

            const auto user_response =
                dynamic_cast<const User*>(auth_response->get_item());
            if (user_response == nullptr) {
                throw std::runtime_error(
                    "Failed to case auth response type to user");
            }
            user = *user_response;
        }
    }
    return func(request);
}
}  // namespace hestia