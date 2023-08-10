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
    const HttpRequest& request, const User& user)
{
    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    if (path.empty() || path == "/") {
        Map action_map;
        request.get_header().get_data().copy_with_prefix(
            "hestia.hsm_action.", action_map);

        if (!action_map.empty()) {
            return do_hsm_action(request, action_map, user);
        }
    }
    return CrudWebView::on_get(request, user);
}

HttpResponse::Ptr HestiaHsmActionView::on_put(
    const HttpRequest& request, const User& user)
{
    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    if (path.empty() || path == "/") {
        Map action_map;
        request.get_header().get_data().copy_with_prefix(
            "hestia.hsm_action.", action_map);

        if (!action_map.empty()) {
            return do_hsm_action(request, action_map, user);
        }
    }
    return CrudWebView::on_put(request, user);
}

HttpResponse::Ptr HestiaHsmActionView::on_delete(
    const HttpRequest& request, const User& user)
{
    return CrudWebView::on_delete(request, user);
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


HttpResponse::Ptr HestiaHsmActionView::do_hsm_action(
    const HttpRequest& request, const Map& action_map, const User& user)
{
    LOG_INFO("Processing HSM Action");

    Dictionary action_dict;
    action_dict.expand(action_map);

    HsmAction action;
    action.deserialize(action_dict);

    auto response = HttpResponse::create();

    if (action.is_data_io_action()) {
        std::string redirect_location;

        auto completion_cb = [&response, &redirect_location](
                                 HsmActionResponse::Ptr response_ret) {
            if (response_ret->ok()) {
                LOG_INFO("Data action completed sucessfully");
                if (!response_ret->get_redirect_location().empty()) {
                    redirect_location = response_ret->get_redirect_location();
                }
            }
            else {
                LOG_ERROR(
                    "Error in data action \n"
                    << response_ret->get_error().to_string());
                response = HttpResponse::create(500, "Internal Server Error.");
            }
        };
        m_hestia_service->do_data_io_action(
            HsmActionRequest(action, user.get_primary_key()),
            request.get_context()->get_stream(), completion_cb);

        if (!redirect_location.empty()) {
            response = HttpResponse::create(307, "Found");
            response->header().set_item(
                "Location", "http://" + redirect_location + m_path);
        }
    }
    else {
        auto action_response = m_hestia_service->make_request(
            HsmActionRequest(action, user.get_primary_key()));

        if (!action_response->ok()) {
            return HttpResponse::create(500, "Internal Server Error.");
        }
    }

    return response;
}

}  // namespace hestia