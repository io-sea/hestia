#include "HestiaHsmActionView.h"

#include "DistributedHsmService.h"
#include "HsmService.h"

#include "HsmServicesFactory.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "RequestContext.h"
#include "StringAdapter.h"

#include "Logger.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "HsmAction.h"

namespace hestia {
HestiaHsmActionView::HestiaHsmActionView(
    DistributedHsmService* hestia_service) :
    CrudWebView(hestia_service->get_hsm_service(), HsmItem::hsm_action_name),
    m_hestia_service(hestia_service)
{
    m_can_stream = true;
}

HestiaHsmActionView::~HestiaHsmActionView() {}

HttpResponse::Ptr HestiaHsmActionView::on_get(
    const HttpRequest& request, const User&)
{
    return do_hsm_action(request);
}

HttpResponse::Ptr HestiaHsmActionView::on_put(
    const HttpRequest& request, const User&)
{
    return do_hsm_action(request);
}

HttpResponse::Ptr HestiaHsmActionView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    (void)m_hestia_service;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaHsmActionView::on_head(
    const HttpRequest& request, const User&)
{
    const auto path =
        StringUtils::split_on_first(request.get_path(), "/objects").second;
    if (path.empty() || path == "/") {
        return HttpResponse::create();
    }
    return HttpResponse::create(404, "Not Found.");
}


HttpResponse::Ptr HestiaHsmActionView::do_hsm_action(const HttpRequest& request)
{
    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    if (path.empty()) {
        Map action_map;
        request.get_header().get_data().copy_with_prefix(
            "hestia.hsm_action.", action_map);

        if (!action_map.empty()) {
            LOG_INFO("Processing HSM Action");

            Dictionary action_dict;
            action_dict.expand(action_map);

            HsmAction action;
            action.deserialize(action_dict);

            if (action.is_data_io_action()) {
                auto response = HttpResponse::create();

                auto completion_cb = [](HsmActionResponse::Ptr response_ret) {
                    if (response_ret->ok()) {
                        LOG_INFO("Data action completed sucessfully");
                    }
                    else {
                        LOG_ERROR(
                            "Error in data action"
                            << response_ret->get_error().to_string());
                    }
                };
                m_hestia_service->get_hsm_service()->do_data_io_action(
                    action, request.get_context()->get_stream(), completion_cb);

                return response;
            }
            else {
                auto action_response =
                    m_hestia_service->get_hsm_service()->make_request(
                        HsmActionRequest(action));
                if (!action_response->ok()) {
                    return HttpResponse::create(500, "Internal Server Error.");
                }
            }
        }
    }
    return HttpResponse::create(404, "Not Found.");
}

}  // namespace hestia