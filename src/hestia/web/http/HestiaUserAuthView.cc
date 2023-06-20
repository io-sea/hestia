#include "HestiaUserAuthView.h"

#include "UserAdapter.h"
#include "UserService.h"

#include "StringUtils.h"

namespace hestia {
HestiaUserAuthView::HestiaUserAuthView(UserService* user_service) :
    WebView(),
    m_user_service(user_service),
    m_user_adapter(std::make_unique<UserJsonAdapter>())
{
}

HestiaUserAuthView::~HestiaUserAuthView() {}

HttpResponse::Ptr HestiaUserAuthView::on_post(
    const HttpRequest& request, const User&)
{
    auto path = request.get_path();
    if (path[path.size() - 1] == '/') {
        path = path.substr(0, path.size() - 1);
    }

    std::string username = request.get_queries().get_item("user");
    std::string password = request.get_queries().get_item("password");
    if (username.empty() || password.empty()) {
        return HttpResponse::create(400, "Bad Request");
    }

    std::unique_ptr<UserServiceResponse> response;

    if (StringUtils::ends_with(path, "/register")) {
        response = m_user_service->register_user(username, password);
    }
    else {
        response = m_user_service->authenticate_user(username, password);
    }
    if (!response->ok()) {
        return HttpResponse::create(500, "Server Error");
    }

    auto http_response = HttpResponse::create();
    std::string body;
    m_user_adapter->to_string(response->item(), body);
    http_response->set_body(body);
    return http_response;
}

}  // namespace hestia