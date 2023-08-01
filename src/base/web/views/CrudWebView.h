#pragma once

#include "CrudService.h"
#include "WebView.h"

namespace hestia {
class CrudWebView : public WebView {
  public:
    CrudWebView(CrudService* service, const std::string& type_name = {}) :
        WebView(), m_service(service), m_type_name(type_name)
    {
    }

    HttpResponse::Ptr on_get(const HttpRequest& request, const User&) override;

    HttpResponse::Ptr on_put(const HttpRequest& request, const User&) override;

  private:
    std::string get_path(const HttpRequest& request) const;

    CrudService* m_service{nullptr};
    std::string m_type_name;
};
}  // namespace hestia