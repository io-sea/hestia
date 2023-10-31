#pragma once

#include "User.h"
#include "WebView.h"
#include <memory>

namespace hestia {
class UserService;
class UserJsonAdapter;

class HestiaUserAuthView : public WebView {
  public:
    HestiaUserAuthView(UserService* user_service);

    ~HestiaUserAuthView();

    HttpResponse::Ptr on_post(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

  private:
    UserService* m_user_service{nullptr};
};
}  // namespace hestia