#include "HestiaUserView.h"

#include "UserService.h"

#include "StringUtils.h"

namespace hestia {
HestiaUserView::HestiaUserView(UserService* user_service) :
    WebView(),
    m_user_service(user_service),
    m_user_adapter(std::make_unique<JsonAdapter<User>>())
{
}

HestiaUserView::~HestiaUserView() {}

HttpResponse::Ptr HestiaUserView::on_get(
    const HttpRequest& request, const User& user)
{
    if (!user.m_is_admin) {
        return HttpResponse::create(403, "Forbidden");
    }

    const auto path =
        StringUtils::split_on_first(request.get_path(), "/users").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Listing available users");
        auto get_response = m_user_service->make_request(CrudMethod::MULTI_GET);
        if (!get_response->ok()) {
            LOG_ERROR(
                "Failed to get users: "
                << get_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_user_adapter->to_string(get_response->items(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaUserView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaUserView::on_head(
    const HttpRequest& request, const User&)
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/users").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}

}  // namespace hestia