#include "HestiaHsmActionView.h"

#include "DistributedHsmService.h"
#include "HsmService.h"

#include "HsmServicesFactory.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ModelSerializer.h"
#include "RequestContext.h"

#include "Logger.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "HsmAction.h"

namespace hestia {
HestiaHsmActionView::HestiaHsmActionView(
    DistributedHsmService* hestia_service) :
    CrudWebView(hestia_service->get_hsm_service(), HsmItem::hsm_action_name),
    m_hestia_service(hestia_service)
{
}

HestiaHsmActionView::~HestiaHsmActionView() {}

HttpResponse::Ptr HestiaHsmActionView::on_get(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (m_needs_auth && auth.m_user_id.empty()) {
        LOG_ERROR("User not found");
        return HttpResponse::create(
            {HttpStatus::Code::_401_UNAUTHORIZED, "User not found."});
    }

    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    if (path.empty() || path == "/") {
        Map action_map;
        request.get_header().get_data().copy_with_prefix(
            {"hestia.hsm_action."}, action_map);

        if (!action_map.empty()) {
            if (event == HttpEvent::HEADERS) {
                return do_hsm_action(request, action_map, auth);
            }
            else {
                return HttpResponse::create();
            }
        }
        else {
            return CrudWebView::on_get(request, event, auth);
        }
    }
    return CrudWebView::on_get(request, event, auth);
}

HttpResponse::Ptr HestiaHsmActionView::on_put(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (m_needs_auth && auth.m_user_id.empty()) {
        LOG_ERROR("User not found");
        return HttpResponse::create(
            {HttpStatus::Code::_401_UNAUTHORIZED, "User not found."});
    }

    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    if (path.empty() || path == "/") {
        Map action_map;
        request.get_header().get_data().copy_with_prefix(
            {"hestia.hsm_action."}, action_map);

        if (!action_map.empty()) {
            if (!request.is_content_outstanding()) {
                return std::make_unique<HttpResponse>(
                    HttpResponse::CompletionStatus::FINISHED);
            }
            else if (event == HttpEvent::HEADERS) {
                return do_hsm_action(request, action_map, auth);
            }
            else {
                if (request.get_context()->has_response()) {
                    return std::make_unique<HttpResponse>(
                        *request.get_context()->get_response());
                }
                else {
                    return HttpResponse::create();
                }
            }
        }
        else {
            return CrudWebView::on_put(request, event, auth);
        }
    }
    return CrudWebView::on_put(request, event, auth);
}

HttpResponse::Ptr HestiaHsmActionView::on_delete(
    const HttpRequest& request,
    HttpEvent event,
    const AuthorizationContext& auth)
{
    if (m_needs_auth && auth.m_user_id.empty()) {
        LOG_ERROR("User not found");
        return HttpResponse::create(
            {HttpStatus::Code::_401_UNAUTHORIZED, "User not found."});
    }

    return CrudWebView::on_delete(request, event, auth);
}

HttpResponse::Ptr HestiaHsmActionView::on_head(
    const HttpRequest& request, HttpEvent, const AuthorizationContext& auth)
{
    if (m_needs_auth && auth.m_user_id.empty()) {
        LOG_ERROR("User not found");
        return HttpResponse::create(
            {HttpStatus::Code::_401_UNAUTHORIZED, "User not found."});
    }

    const auto path =
        StringUtils::split_on_first(request.get_path(), "/objects").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}

HttpResponse::Ptr HestiaHsmActionView::do_hsm_action(
    const HttpRequest& request,
    const Map& action_map,
    const AuthorizationContext& auth)
{
    LOG_INFO("Processing HSM Action - token is: " + auth.m_user_token);

    Dictionary action_dict;
    action_dict.expand(action_map);

    HsmAction action;
    action.deserialize(action_dict);

    if (action.get_subject_key().empty()) {
        return HttpResponse::create(
            {HttpStatus::Code::_400_BAD_REQUEST,
             "Missing subject key in action header"});
    }

    auto response = HttpResponse::create(
        HttpResponse::CompletionStatus::AWAITING_BODY_CHUNK);

    std::string redirect_location;
    auto completion_cb =
        [request_context = request.get_context(),
         &redirect_location](HsmActionResponse::Ptr response_ret) {
            if (response_ret->ok()) {
                LOG_INFO("Data action completed sucessfully");
                if (!response_ret->get_redirect_location().empty()) {
                    redirect_location = response_ret->get_redirect_location();
                }
                else {
                    auto response = HttpResponse::create();

                    ModelSerializer serializer(
                        std::make_unique<TypedModelFactory<HsmAction>>());
                    Dictionary dict;
                    serializer.to_dict(response_ret->get_action(), dict);
                    response->set_body(JsonDocument(dict).to_string());
                    request_context->set_response(std::move(response));
                }
            }
            else {
                const std::string msg = "Error in data action \n"
                                        + response_ret->get_error().to_string();
                LOG_ERROR(msg);
                request_context->set_response(HttpResponse::create(
                    {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR, msg}));
            }
        };

    m_hestia_service->do_hsm_action(
        HsmActionRequest(action, {auth.m_user_id, auth.m_user_token}),
        request.get_context()->get_stream(), completion_cb);

    if (!redirect_location.empty()) {
        response = HttpResponse::create(307, "Found");
        response->header().set_item(
            "Location", "http://" + redirect_location + m_path);
    }

    if (request.get_context()->has_response()
        && request.get_context()->get_response()->error()) {
        return std::make_unique<HttpResponse>(
            *request.get_context()->get_response());
    }
    return response;
}

}  // namespace hestia