#include "HestiaTierView.h"

#include "DistributedHsmService.h"
#include "HsmService.h"

#include "StorageTierAdapter.h"
#include "TierService.h"

#include "StringUtils.h"

namespace hestia {
HestiaTierView::HestiaTierView(DistributedHsmService* hestia_service) :
    WebView(),
    m_hestia_service(hestia_service),
    m_tier_adapter(std::make_unique<StorageTierJsonAdapter>())
{
}

HestiaTierView::~HestiaTierView() {}

HttpResponse::Ptr HestiaTierView::on_get(
    const HttpRequest& request, const User&)
{
    auto tier_service = m_hestia_service->get_hsm_service()->get_tier_service();
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/tiers").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Listing available tiers");
        auto get_response = tier_service->make_request(CrudMethod::MULTI_GET);
        if (!get_response->ok()) {
            LOG_ERROR(
                "Failed to get tiers: "
                << get_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_tier_adapter->to_string(get_response->items(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaTierView::on_put(
    const HttpRequest& request, const User&)
{
    auto tier_service = m_hestia_service->get_hsm_service()->get_tier_service();
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/tiers").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Creating tier");
        StorageTier tier;
        if (!request.body().empty()) {
            m_tier_adapter->from_string(request.body(), tier);
        }

        auto put_response = tier_service->make_request({tier, CrudMethod::PUT});
        if (!put_response->ok()) {
            LOG_ERROR(
                "Failed to put tier: "
                << put_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_tier_adapter->to_string(put_response->item(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaTierView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaTierView::on_head(
    const HttpRequest& request, const User&)
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/tiers").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}

}  // namespace hestia