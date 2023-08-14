#include "S3AuthenticationMiddleware.h"

#include "S3AuthorisationSession.h"

namespace hestia {
HttpResponse::Ptr S3AuthenticationMiddleware::call(
    const HttpRequest& request,
    AuthorizationContext& auth,
    HttpEvent event,
    responseProviderFunc func)
{
    if (event != HttpEvent::HEADERS) {
        return func(request);
    }

    auto auth_session = S3AuthorisationSession(m_user_service);
    const auto status = auth_session.authorise(request);
    if (status.is_valid()) {
        auth.m_user_id    = status.m_user_internal_id;
        auth.m_user_token = status.m_user_key;
        LOG_INFO("Authenticated user: " + auth.m_user_id);
    }
    else {
        LOG_INFO("Did not authenticate user: " + status.to_string());
    }
    return func(request);
}
}  // namespace hestia