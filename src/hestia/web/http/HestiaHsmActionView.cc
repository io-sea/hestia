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
    m_can_stream = false;  // TODO: Full streaming support in View
}

HestiaHsmActionView::~HestiaHsmActionView() {}

HttpResponse::Ptr HestiaHsmActionView::on_get(const HttpRequest&, const User&)
{
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaHsmActionView::on_put(
    const HttpRequest& request, const User&)
{
    auto path = StringUtils::split_on_first(
                    request.get_path(),
                    "/" + std::string(HsmItem::hsm_action_name) + "s")
                    .second;
    hestia::StringUtils::remove_prefix(path, "/");

    // todo split this into put and get separately. If the HSM action is found
    // in the header then the body (if hsm put or get) is the data stream,
    // otherwise treat it as a regular CRUD request for the HSM action type.

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
                Stream stream;
                std::vector<char> content;
                std::unique_ptr<WriteableBufferView> buffer_view;

                if (action.is_data_put_action()) {
                    auto source = std::make_unique<InMemoryStreamSource>(
                        ReadableBufferView(request.body()));
                    stream.set_source(std::move(source));
                    stream.set_source_size(request.body().size());
                }
                else {
                    content.resize(action.get_size());
                    buffer_view =
                        std::make_unique<WriteableBufferView>(content);
                    auto sink =
                        std::make_unique<InMemoryStreamSink>(*buffer_view);
                    stream.set_sink(std::move(sink));
                }

                HsmActionResponse::Ptr data_io_response;
                auto completion_cb =
                    [&data_io_response](HsmActionResponse::Ptr response_ret) {
                        data_io_response = std::move(response_ret);
                    };
                m_hestia_service->get_hsm_service()->do_data_io_action(
                    action, &stream, completion_cb);
                auto stream_state = stream.flush();
                if (!stream_state.ok()) {
                    LOG_ERROR(
                        "Failed to stream content: "
                        << stream_state.to_string());
                    return HttpResponse::create(500, "Internal Server Error.");
                }
                if (!data_io_response->ok()) {
                    LOG_ERROR(
                        "Failed to io object data: "
                        << data_io_response->get_error().to_string());
                    return HttpResponse::create(500, "Internal Server Error.");
                }

                if (!action.is_data_put_action()) {
                    response->set_body({content.begin(), content.end()});
                }

                // std::string body;
                // m_adapter->to_string(data_put_response->items(), body);
                // response->set_body(body);
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

}  // namespace hestia