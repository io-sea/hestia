#include "HestiaNodeView.h"

#include "DistributedHsmService.h"
#include "HsmNode.h"
#include "StringAdapter.h"

#include "InMemoryStreamSink.h"
#include "RequestContext.h"

#include "StringUtils.h"

#include "Logger.h"

namespace hestia {
HestiaNodeView::HestiaNodeView(DistributedHsmService* hestia_service) :
    WebView(), m_hestia_service(hestia_service)
{
}

HttpResponse::Ptr HestiaNodeView::on_get(
    const HttpRequest& request, const User&)
{
    auto response = HttpResponse::create();

    auto full_path     = request.get_path();
    auto relative_path = StringUtils::remove_prefix(full_path, m_path);

    LOG_INFO("Full path: " + full_path);
    LOG_INFO("View path: " + m_path);
    LOG_INFO("Relative path: " + relative_path);

    if (relative_path.empty() || relative_path == "/") {

        auto get_response = m_hestia_service->make_request(
            DistributedHsmServiceRequestMethod::GET);
        if (!get_response->ok()) {
            return HttpResponse::create(500, "Internal Server Error");
        }

        std::string body;
        JsonAdapter<HsmNode>().to_string(get_response->items(), body);
        response->set_body(body);
    }
    return response;
}

HttpResponse::Ptr HestiaNodeView::on_put(
    const HttpRequest& request, const User&)
{
    LOG_INFO("Into nodeview put");
    auto response = HttpResponse::create();

    auto full_path     = request.get_path();
    auto relative_path = StringUtils::remove_prefix(full_path, m_path);

    if (relative_path.empty() || relative_path == "/") {
        if (!request.body().empty()) {
            HsmNode node;
            LOG_INFO("Trying to serialize: " << request.body());
            JsonAdapter<HsmNode>().from_string(request.body(), node);
            node.reset_id();

            auto put_response = m_hestia_service->make_request(
                {node, DistributedHsmServiceRequestMethod::PUT});
            if (!put_response->ok()) {
                return HttpResponse::create(500, "Internal Server Error");
            }

            std::string body;
            JsonAdapter<HsmNode>().to_string(put_response->item(), body);
            response->set_body(body);
        }
    }
    return response;
}

HttpResponse::Ptr HestiaNodeView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaNodeView::on_head(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

}  // namespace hestia