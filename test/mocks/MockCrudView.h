#pragma once

#include "CrudService.h"
#include "WebView.h"

namespace hestia::mock {
class MockCrudView : public WebView {
  public:
    MockCrudView(CrudService* service) : WebView(), m_service(service) {}

    HttpResponse::Ptr on_get(const HttpRequest& request, const User&) override;

    HttpResponse::Ptr on_put(const HttpRequest& request, const User&) override;

  private:
    std::string get_path(const HttpRequest& request) const;

    CrudService* m_service{nullptr};
};
}  // namespace hestia::mock