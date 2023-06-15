#include "HestiaObjectView.h"

#include "DistributedHsmService.h"
#include "HsmService.h"

#include "HsmObjectAdapter.h"
#include "ObjectService.h"

#include "Logger.h"
#include "StringUtils.h"

namespace hestia {
HestiaObjectView::HestiaObjectView(DistributedHsmService* hestia_service) :
    WebView(),
    m_hestia_service(hestia_service),
    m_object_adapter(std::make_unique<HsmObjectJsonAdapter>("hestia"))
{
}

HestiaObjectView::~HestiaObjectView() {}

HttpResponse::Ptr HestiaObjectView::on_get(const HttpRequest& request)
{
    auto object_service =
        m_hestia_service->get_hsm_service()->get_object_service();
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/objects").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Listing available objects");
        auto object_get_response =
            object_service->make_request(CrudMethod::MULTI_GET);
        if (!object_get_response->ok()) {
            LOG_ERROR(
                "Failed to get objects: "
                << object_get_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_object_adapter->to_string(object_get_response->items(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaObjectView::on_put(const HttpRequest& request)
{
    auto object_service =
        m_hestia_service->get_hsm_service()->get_object_service();
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/objects").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Creating object");
        HsmObject obj;
        if (!request.body().empty()) {
            m_object_adapter->from_string(request.body(), obj);
        }

        auto object_put_response =
            object_service->make_request({obj, CrudMethod::PUT});
        if (!object_put_response->ok()) {
            LOG_ERROR(
                "Failed to put object: "
                << object_put_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_object_adapter->to_string(object_put_response->item(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaObjectView::on_delete(const HttpRequest& request)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaObjectView::on_head(const HttpRequest& request)
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/objects").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}

}  // namespace hestia