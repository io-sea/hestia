#pragma once

#include "CrudWebView.h"
#include "HsmObject.h"

#include <memory>

namespace hestia {

class DistributedHsmService;

class HestiaHsmActionView : public CrudWebView {
  public:
    HestiaHsmActionView(DistributedHsmService* hestia_service);

    ~HestiaHsmActionView();

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

    HttpResponse::Ptr on_head(
        const HttpRequest& request,
        HttpEvent event,
        const AuthorizationContext& auth) override;

  private:
    HttpResponse::Ptr do_hsm_action(
        const HttpRequest& request,
        const Map& action_map,
        const AuthorizationContext& auth);

    DistributedHsmService* m_hestia_service{nullptr};
};
}  // namespace hestia