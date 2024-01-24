#include "S3AuthorisationChecker.h"

#include "HashUtils.h"
#include "Logger.h"
#include "S3Path.h"

namespace hestia {

S3AuthorisationChecker::AuthResponse S3AuthorisationChecker::authorise(
    const UserService& user_service,
    const HttpRequest& http_request,
    const std::string& domain,
    bool check_signature)
{
    S3Request s3_request(http_request, domain, true);

    if (!s3_request.is_valid()) {
        LOG_ERROR(
            "Failed parsing request auth headers: " + s3_request.to_string());
        return {
            Status::FAILED,
            {s3_request.get_status_code(), s3_request},
            UserContext()};
    }

    const auto& [user_status, user_context] =
        find_user(user_service, s3_request);
    if (!user_status.is_ok()) {
        LOG_ERROR("Failed to find user");
        return {Status::FAILED, user_status, user_context};
    }

    if (awaiting_signed_payload(http_request)) {
        return {Status::WAITING_FOR_PAYLOAD, {}, user_context};
    }

    s3_request.set_user_secret_key(user_context.m_token);

    const auto check_status = check(s3_request, http_request, check_signature);
    return {
        check_status.is_ok() ? Status::VALID : Status::FAILED, check_status,
        user_context};
}

std::pair<S3Status, S3AuthorisationChecker::UserContext>
S3AuthorisationChecker::find_user(
    const UserService& user_service, S3Request& s3_request)
{
    CrudQuery query(
        KeyValuePair{"name", s3_request.get_user_context().m_user_id},
        {CrudQuery::BodyFormat::ITEM}, CrudQuery::Format::GET);
    auto response =
        user_service.make_request(CrudRequest{CrudMethod::READ, query, {}});

    if (!response->ok()) {
        LOG_ERROR(s3_request.to_string())
        return {{S3StatusCode::_403_ACCESS_DENIED, s3_request}, UserContext()};
    }

    if (!response->found()) {
        LOG_ERROR(s3_request.to_string())
        return {{S3StatusCode::_403_INVALID_KEY_ID, s3_request}, UserContext()};
    }

    const auto user_item = response->get_item_as<User>();

    UserContext user_context;
    user_context.m_id = user_item->get_primary_key();
    if (user_item->tokens().empty()) {
        LOG_ERROR(s3_request.to_string())
        return {{S3StatusCode::_403_ACCESS_DENIED, s3_request}, user_context};
    }
    else {
        user_context.m_token = user_item->get_first_token().value();
        return {{}, user_context};
    }
}

bool S3AuthorisationChecker::awaiting_signed_payload(const HttpRequest& request)
{
    if (!request.is_content_outstanding()) {
        return false;
    }

    const auto payload_sha256 =
        request.get_header().get_item("x-amz-content-sha256");
    if (payload_sha256.empty() || payload_sha256 == "UNSIGNED-PAYLOAD") {
        return false;
    }
    return true;
}

S3Status S3AuthorisationChecker::check(
    const S3Request& s3_request,
    const HttpRequest& http_request,
    bool check_signature)
{
    const auto signature = s3_request.get_signature(
        http_request, HashUtils::do_sha256(http_request.body()),
        s3_request.get_user_context().m_user_secret_key);
    if ((!check_signature) || (s3_request.m_signature == signature)) {
        return {};
    }
    else {
        LOG_ERROR(
            "Signatures do not match: " << s3_request.m_signature << " | "
                                        << signature << "\n"
                                        << s3_request.to_string());
        return {S3StatusCode::_403_ACCESS_DENIED, s3_request};
    }
}

}  // namespace hestia
