#pragma once

#include "CrudService.h"
#include "WebView.h"

namespace hestia {
class CrudWebView : public WebView {
  public:
    CrudWebView(CrudService* service, const std::string& type_name = {});

    HttpResponse::Ptr on_get(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

  private:
    std::string get_path(const HttpRequest& request) const;

    CrudQuery::ChildFormat get_child_format(
        const std::string& query_value) const;

    CrudService* m_service{nullptr};
    std::string m_type_name;

    CrudQuery::FormatSpec m_readable_format;
    CrudQuery::FormatSpec m_machine_format;
    CrudQuery::ChildFormat m_default_child_format{CrudQuery::ChildFormat::FULL};
};
}  // namespace hestia