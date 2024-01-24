#include "S3AuthenticationMiddleware.h"

#include "S3AuthorisationChecker.h"

namespace hestia {

S3AuthenticationMiddleware::S3AuthenticationMiddleware(
    const std::string& domain, bool enable_auth) :
    m_enable_auth(enable_auth), m_domain(domain)
{
}

HttpResponse::Ptr S3AuthenticationMiddleware::call(
    const HttpRequest& request,
    AuthorizationContext& auth,
    HttpEvent event,
    responseProviderFunc func)
{
    if (event == HttpEvent::BODY || event == HttpEvent::CONNECTED) {
        return func(request);
    }

    const auto& auth_response = S3AuthorisationChecker::authorise(
        *m_user_service, request, m_domain, m_enable_auth);

    if (auth_response.m_status == S3AuthorisationChecker::Status::FAILED) {
        LOG_INFO(
            "Did not authenticate user: "
            + auth_response.m_s3_status.to_string());

        const auto& [code, id] = auth_response.m_s3_status.get_code_and_id();
        auto response          = HttpResponse::create(code, id);
        response->set_body(auth_response.m_s3_status.to_string());
        return response;
    }
    else if (
        auth_response.m_status
        == S3AuthorisationChecker::Status::WAITING_FOR_PAYLOAD) {
        if (event == HttpEvent::EOM) {
            LOG_ERROR("Still waiting for payload at EOM");
            return HttpResponse::create(
                HttpStatus::Code::_500_INTERNAL_SERVER_ERROR);
        }
        else {
            auto response = HttpResponse::create();
            LOG_INFO("Auth waiting for EOM for signed payload");
            response->set_completion_status(
                HttpResponse::CompletionStatus::AWAITING_EOM);
            return response;
        }
    }
    else {
        auth.m_user_id    = auth_response.m_user.m_id;
        auth.m_user_token = auth_response.m_user.m_token;

        LOG_INFO("Authenticated user: " + auth.m_user_id);
        return func(request);
    }
}
}  // namespace hestia