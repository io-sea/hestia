#include "HestiaUserAuthView.h"

#include "UserService.h"

#include "HttpParser.h"
#include "ModelSerializer.h"
#include "StringUtils.h"
#include "User.h"

namespace hestia {
HestiaUserAuthView::HestiaUserAuthView(UserService* user_service) :
    WebView(), m_user_service(user_service)
{
}

HestiaUserAuthView::~HestiaUserAuthView() {}

HttpResponse::Ptr HestiaUserAuthView::on_post(
    const HttpRequest& request, HttpEvent, const AuthorizationContext&)
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
        const auto msg = "Couldn't parse username or password from request";
        LOG_ERROR(msg);
        return HttpResponse::create(
            HttpStatus{HttpStatus::Code::_400_BAD_REQUEST, msg});
    }

    CrudResponse::Ptr response;

    const bool is_register_flow = StringUtils::ends_with(path, "/register");
    if (is_register_flow) {
        response = m_user_service->register_user(username, password);
    }
    else {
        response = m_user_service->authenticate_user(username, password);
    }
    if (!response->ok()) {
        const auto msg = "Server Error In User registration: "
                         + response->get_error().to_string();
        return HttpResponse::create(
            {HttpStatus::Code::_500_INTERNAL_SERVER_ERROR, msg});
    }
    if (!is_register_flow && !response->found()) {
        const auto msg = "Requested user: " + username + " not found.";
        return HttpResponse::create({HttpStatus::Code::_404_NOT_FOUND, msg});
    }

    auto http_response = HttpResponse::create();

    auto adapter = std::make_unique<ModelSerializer>(
        std::make_unique<TypedModelFactory<User>>());

    Dictionary dict;
    adapter->to_dict(response->items(), dict);
    http_response->set_body(JsonDocument(dict).to_string());

    LOG_INFO(
        "Found user ok - responding with user body: " << http_response->body());
    return http_response;
}

}  // namespace hestia