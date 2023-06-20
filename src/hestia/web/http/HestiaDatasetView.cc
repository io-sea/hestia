#include "HestiaDatasetView.h"

#include "DatasetAdapter.h"
#include "DatasetService.h"

#include "StringUtils.h"

namespace hestia {
HestiaDatasetView::HestiaDatasetView(DatasetService* user_service) :
    WebView(),
    m_dataset_service(user_service),
    m_dataset_adapter(std::make_unique<DatasetJsonAdapter>())
{
}

HestiaDatasetView::~HestiaDatasetView() {}

HttpResponse::Ptr HestiaDatasetView::on_get(
    const HttpRequest& request, const User& user)
{
    if (!user.m_is_admin) {
        return HttpResponse::create(403, "Forbidden");
    }

    const auto path =
        StringUtils::split_on_first(request.get_path(), "/datasets").second;

    if (path.empty() || path == "/") {
        LOG_INFO("Listing available datasets");
        auto get_response =
            m_dataset_service->make_request(CrudMethod::MULTI_GET);
        if (!get_response->ok()) {
            LOG_ERROR(
                "Failed to get datasets: "
                << get_response->get_error().to_string());
            return HttpResponse::create(500, "Internal Server Error.");
        }
        auto response = HttpResponse::create();
        std::string body;
        m_dataset_adapter->to_string(get_response->items(), body);
        response->set_body(body);
        return response;
    }
    else {
        auto response = HttpResponse::create();
        response->set_body("Hello world");
        return response;
    }
}

HttpResponse::Ptr HestiaDatasetView::on_put(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaDatasetView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaDatasetView::on_head(
    const HttpRequest& request, const User&)
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/datasets").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}

}  // namespace hestia