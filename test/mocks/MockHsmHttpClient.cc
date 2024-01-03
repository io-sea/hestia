#include "MockHsmHttpClient.h"

#include "HestiaHsmActionView.h"
#include "PingView.h"
#include "RequestContext.h"
#include "UrlRouter.h"

MockHsmWebApp::MockHsmWebApp(hestia::DistributedHsmService* hsm_service) :
    WebApp(hsm_service->get_user_service())
{
    const std::string api_prefix = "/api/v1/";

    m_url_router = std::make_unique<hestia::UrlRouter>();

    m_url_router->add_pattern(
        {api_prefix + hestia::HsmItem::hsm_action_name + "s"},
        std::make_unique<hestia::HestiaHsmActionView>(hsm_service));

    m_url_router->add_pattern(
        {api_prefix + "ping"}, std::make_unique<hestia::PingView>());

    auto middleware   = std::make_unique<MockHsmMiddleware>();
    auto current_user = hsm_service->get_user_service()->get_current_user();

    middleware->m_test_user.m_user_id    = current_user.get_primary_key();
    middleware->m_test_user.m_user_token = current_user.tokens()[0].value();
    add_middleware(std::move(middleware));
}

void MockHsmHttpClient::make_request(
    const hestia::HttpRequest& request,
    hestia::HttpClient::completionFunc completion_func,
    hestia::Stream* stream,
    std::size_t,
    progressFunc)
{
    LOG_INFO("Got request to: " << request.get_path());

    const auto stripped_path =
        hestia::StringUtils::remove_prefix(request.get_path(), "http://");

    std::string working_address;
    MockHsmWebApp* working_app{nullptr};
    for (const auto& [address, app] : m_apps) {
        if (hestia::StringUtils::starts_with(stripped_path, address)) {
            working_address = address;
            working_app     = app.get();
        }
    }

    if (working_app == nullptr) {
        completion_func(hestia::HttpResponse::create(404, "Not Found"));
    }

    auto intercepted_request = request;
    intercepted_request.overwrite_path(
        hestia::StringUtils::remove_prefix(stripped_path, working_address));

    hestia::RequestContext request_context;
    request_context.set_request(intercepted_request);

    working_app->on_event(&request_context, hestia::HttpEvent::HEADERS);
    if (request_context.get_response()->code() == 307) {
        working_address = hestia::StringUtils::remove_prefix(
            request_context.get_response()->header().get_item("location"),
            "http://");
        const auto& [start, rest] =
            hestia::StringUtils::split_on_first(working_address, "/");
        working_address = start;

        LOG_INFO("Got redirect to: " + working_address);
        auto app_iter = m_apps.find(working_address);
        working_app   = nullptr;
        if (app_iter != m_apps.end()) {
            working_app = app_iter->second.get();
        }
        if (working_app == nullptr) {
            completion_func(hestia::HttpResponse::create(404, "Not Found"));
            return;
        }
        working_app->on_event(&request_context, hestia::HttpEvent::HEADERS);
    }

    working_app->on_event(&request_context, hestia::HttpEvent::EOM);

    LOG_INFO("Has stream - proceeding? " << bool(stream));

    if (stream != nullptr) {
        if (request.get_method() == hestia::HttpRequest::Method::PUT) {
            std::vector<char> buffer(1024);
            hestia::WriteableBufferView buffer_view(buffer);
            auto read_result = stream->read(buffer_view);
            if (read_result.m_num_transferred > 0) {
                auto write_result = request_context.get_stream()->write(
                    hestia::ReadableBufferView(
                        &buffer[0], read_result.m_num_transferred));
                LOG_INFO(
                    "Wrote: " << write_result.m_num_transferred << " bytes.");
                if (!write_result.ok()) {
                    LOG_ERROR("Bad write result");
                }
                LOG_INFO("About to reset stream");
                if (!request_context.get_stream()->reset().ok()) {
                    LOG_ERROR("Bad stream reset");
                }
            }
        }
        else if (request.get_method() == hestia::HttpRequest::Method::GET) {
            std::vector<char> buffer(1024);
            hestia::WriteableBufferView buffer_view(buffer);
            auto read_result = request_context.get_stream()->read(buffer_view);
            LOG_INFO(
                "Read: " << read_result.m_num_transferred << " bytes: "
                         << std::string(buffer.begin(), buffer.end()));
            if (read_result.m_num_transferred > 0) {
                auto write_result = stream->write(hestia::ReadableBufferView(
                    buffer.data(), read_result.m_num_transferred));
                LOG_INFO(
                    "Wrote: " << write_result.m_num_transferred << " bytes.");
                if (!write_result.ok()) {
                    LOG_ERROR("Bad write result");
                }
            }
        }
    }
    auto response =
        std::make_unique<hestia::HttpResponse>(*request_context.get_response());
    LOG_INFO("Setting location header to: " << working_address);
    response->header().set_item("location", working_address);
    completion_func(std::move(response));
}