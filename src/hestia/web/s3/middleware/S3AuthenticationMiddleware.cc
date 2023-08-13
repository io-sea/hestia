#include "S3AuthenticationMiddleware.h"

#include "S3AuthorisationSession.h"

namespace hestia {
HttpResponse::Ptr S3AuthenticationMiddleware::call(
    const HttpRequest& request,
    AuthorizationContext&,
    HttpEvent,
    responseProviderFunc func)
{
    auto auth_session = S3AuthorisationSession(m_user_service);

    const auto status = auth_session.authorise(request);

    /*
    if (status != deimos::protocol::s3::S3Authorisation::Status::VALID)
    {
        auto error = auth->getError();
        HttpResponse response;

        proxygen::ResponseBuilder(downstream_)
            .status(
                auth_error->https_error_code,
                auth_error->https_error_identifier)
            .body(
                auth_error->get_error_response_body(m_header->getIdentifier()))
            .sendWithEOM();

        spdlog::warn(
            "authorisation is invalid: {} ({})",
            auth_error->https_error_identifier, auth_error->https_error_code);
        return response;
    }
    */

    return func(request);
}
}  // namespace hestia