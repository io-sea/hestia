#pragma once

#include "WebView.h"
#include <memory>

namespace hestia {
class UserService;
class UserJsonAdapter;

class HestiaUserView : public WebView {
  public:
    HestiaUserView(UserService* user_service);

    ~HestiaUserView();

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user) override;

  private:
    UserService* m_user_service{nullptr};
    std::unique_ptr<UserJsonAdapter> m_user_adapter;
};
}  // namespace hestia