#include "HestiaUserAuthView.h"

#include "UserService.h"

#include "HttpParser.h"
#include "StringUtils.h"

namespace hestia {
HestiaUserAuthView::HestiaUserAuthView(UserService* user_service) :
    WebView(),
    m_user_service(user_service),
    m_adapter(std::make_unique<JsonAdapter>(nullptr))
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

    std::string username;
    std::string password;
    if (!request.get_queries().empty()) {
        username = request.get_queries().get_item("user");
        password = request.get_queries().get_item("password");
    }
    else {
        Map form_data;
        HttpParser::parse_form_data(request.body(), form_data);
        username = form_data.get_item("user");
        password = form_data.get_item("password");
    }

    if (username.empty() || password.empty()) {
        return HttpResponse::create(400, "Bad Request");
    }

    CrudResponse::Ptr response;

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
    m_adapter->to_string(response->items(), body);
    http_response->set_body(body);
    return http_response;
}

}  // namespace hestia