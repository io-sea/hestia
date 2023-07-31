#pragma once

#include "CrudWebView.h"
#include "HsmObject.h"
#include "StringAdapter.h"

#include <memory>

namespace hestia {

class DistributedHsmService;

class HestiaHsmActionView : public CrudWebView {
  public:
    HestiaHsmActionView(DistributedHsmService* hestia_service);

    ~HestiaHsmActionView();

    HttpResponse::Ptr on_get(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_put(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_delete(
        const HttpRequest& request, const User& user) override;

    HttpResponse::Ptr on_head(
        const HttpRequest& request, const User& user) override;

  private:
    HttpResponse::Ptr do_hsm_action(
        const HttpRequest& request, const User& user);

    DistributedHsmService* m_hestia_service{nullptr};
};
}  // namespace hestia