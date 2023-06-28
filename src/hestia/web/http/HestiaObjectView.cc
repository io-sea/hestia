#include "HestiaObjectView.h"

#include "DistributedHsmService.h"
#include "HsmService.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ObjectService.h"
#include "StringAdapter.h"

#include "Logger.h"
#include "StringUtils.h"
#include "UuidUtils.h"

namespace hestia {
HestiaObjectView::HestiaObjectView(DistributedHsmService* hestia_service) :
    WebView(),
    m_hestia_service(hestia_service),
    m_object_adapter(std::make_unique<JsonAdapter<HsmObject>>(
        Model::SerializeFormat::CHILD_ID))
{
}

HestiaObjectView::~HestiaObjectView() {}

HttpResponse::Ptr HestiaObjectView::on_get(
    const HttpRequest& request, const User&)
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
        auto working_path = path;
        if (path[0] == '/') {
            working_path = path.substr(1, path.size() - 1);
        }
        const auto& [obj_id, tier] =
            StringUtils::split_on_first(working_path, '/');
        if (tier.empty()) {
            LOG_INFO("Getting single object: " << obj_id);
            auto get_response = object_service->make_request(
                {HsmObject(UuidUtils::from_string(obj_id)), CrudMethod::GET});
            if (!get_response->ok()) {
                LOG_ERROR(
                    "Failed to get object: "
                    << get_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }
            auto response = HttpResponse::create();
            std::string body;
            m_object_adapter->to_string(get_response->item(), body);
            response->set_body(body);
            return response;
        }
        else {
            LOG_INFO(
                "Getting data for object: " << obj_id << " : "
                                            << request.body());

            const auto obj_uuid = UuidUtils::from_string(obj_id);

            auto get_response = object_service->make_request(
                {HsmObject(obj_uuid), CrudMethod::GET});
            if (!get_response->ok()) {
                LOG_ERROR(
                    "Failed to get object: "
                    << get_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }

            HsmServiceRequest req(
                get_response->item(), HsmServiceRequestMethod::GET);
            req.set_source_tier(std::stoi(tier));

            std::vector<char> body(get_response->item().object().m_size);
            WriteableBufferView buffer_view(body);
            Stream stream;
            auto sink =
                std::make_unique<InMemoryStreamSink>(WriteableBufferView(body));
            stream.set_sink(std::move(sink));

            auto data_get_response =
                m_hestia_service->get_hsm_service()->make_request(req, &stream);
            if (!data_get_response->ok()) {
                LOG_ERROR(
                    "Failed to get object data: "
                    << data_get_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }
            auto stream_state = stream.flush();
            if (!stream_state.ok()) {
                LOG_ERROR(
                    "Failed to stream content: " << stream_state.to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }

            auto response = HttpResponse::create();
            response->set_body({body.begin(), body.end()});
            return response;
        }
    }
}

HttpResponse::Ptr HestiaObjectView::on_put(
    const HttpRequest& request, const User&)
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
        obj.reset_id();

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
        auto working_path = path;
        if (path[0] == '/') {
            working_path = path.substr(1, path.size() - 1);
        }
        const auto& [obj_id, tier] =
            StringUtils::split_on_first(working_path, '/');
        if (!tier.empty()) {
            LOG_INFO(
                "Putting data for object: " << obj_id << " : "
                                            << request.body());

            auto get_response = object_service->make_request(
                {HsmObject(UuidUtils::from_string(obj_id)), CrudMethod::GET});
            if (!get_response->ok()) {
                LOG_ERROR(
                    "Failed to get object: "
                    << get_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }

            HsmServiceRequest req(
                get_response->item().object(), HsmServiceRequestMethod::PUT);
            req.set_should_put_overwrite(true);

            Stream stream;
            auto source = std::make_unique<InMemoryStreamSource>(
                ReadableBufferView(request.body()));
            stream.set_source(std::move(source));
            stream.set_source_size(request.body().size());

            auto data_put_response =
                m_hestia_service->get_hsm_service()->make_request(req, &stream);
            if (!data_put_response->ok()) {
                LOG_ERROR(
                    "Failed to put object data: "
                    << data_put_response->get_error().to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }
            auto stream_state = stream.flush();
            if (!stream_state.ok()) {
                LOG_ERROR(
                    "Failed to stream content: " << stream_state.to_string());
                return HttpResponse::create(500, "Internal Server Error.");
            }

            auto response = HttpResponse::create();
            std::string body;
            m_object_adapter->to_string(data_put_response->object(), body);
            response->set_body(body);
            return response;
        }
        else {
            return HttpResponse::create(404, "Not Found.");
        }
    }
}

HttpResponse::Ptr HestiaObjectView::on_delete(
    const HttpRequest& request, const User&)
{
    (void)request;
    return HttpResponse::create();
}

HttpResponse::Ptr HestiaObjectView::on_head(
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