#include "HttpClient.h"

#include "DistributedHsmServiceTestWrapper.h"
#include "HestiaS3WebApp.h"
#include "RequestContext.h"
#include "S3Request.h"

namespace hestia::mock {
class MockHttpClientForS3 : public hestia::HttpClient {
  public:
    MockHttpClientForS3(const std::string& domain)
    {
        m_domain = domain;

        m_fixture = std::make_unique<DistributedHsmServiceTestWrapper>();
        m_fixture->init(m_user_name, "my_admin_password", 5);

        hestia::HestiaS3WebAppConfig app_config;
        app_config.m_domain = m_domain;
        m_web_app           = std::make_unique<hestia::HestiaS3WebApp>(
            app_config, m_fixture->m_dist_hsm_service.get(),
            m_fixture->m_user_service.get());
    }

    void add_s3_headers(hestia::HttpRequest& req)
    {
        hestia::S3Request s3_request(req, m_domain);
        s3_request.set_user_id(m_user_name);
        s3_request.set_user_secret_key(m_fixture->get_user_token());
        s3_request.m_signed_headers = {
            "host", "x-amz-content-sha256", "x-amz-date"};
        s3_request.populate_headers("", req.get_header());
        s3_request.populate_authorization_headers(
            hestia::S3Request::PayloadSignatureType::UNSIGNED, req);
    }

    void make_request(
        const HttpRequest& request,
        completionFunc completion_func,
        Stream* stream                = nullptr,
        std::size_t progress_interval = 0,
        progressFunc progess_func     = nullptr) override
    {
        (void)progress_interval;
        (void)progess_func;

        m_working_context = std::make_unique<hestia::RequestContext>();
        m_working_context->set_request(request);

        if (m_should_apply_headers) {
            add_s3_headers(m_working_context->get_writeable_request());
        }

        if (stream != nullptr) {
            if (request.get_method() == hestia::HttpRequest::Method::GET) {
                m_working_context->set_output_chunk_handler(
                    [stream](
                        const hestia::ReadableBufferView& buffer,
                        bool finished) {
                        (void)finished;
                        const auto write_result = stream->write(buffer);
                        return write_result.m_num_transferred;
                    });

                m_working_context->set_output_complete_handler(
                    [](const hestia::HttpResponse* response) {
                        REQUIRE(response->code() == 200);
                    });

                m_web_app->on_event(
                    m_working_context.get(), hestia::HttpEvent::HEADERS);

                if (m_working_context->get_response()->get_completion_status()
                    != hestia::HttpResponse::CompletionStatus::FINISHED) {
                    m_web_app->on_event(
                        m_working_context.get(), hestia::HttpEvent::EOM);
                }
                auto response = m_working_context->get_response();
                REQUIRE(response->code() == 200);
                m_working_context->flush_stream();
            }

            else if (request.get_method() == hestia::HttpRequest::Method::PUT) {

                if (m_should_apply_headers) {
                    m_working_context->get_writeable_request()
                        .get_header()
                        .set_item(
                            "Content-Length",
                            std::to_string(stream->get_source_size()));
                }

                m_web_app->on_event(
                    m_working_context.get(), hestia::HttpEvent::HEADERS);

                if (m_working_context->get_response()->get_completion_status()
                    != hestia::HttpResponse::CompletionStatus::FINISHED) {

                    std::size_t chunk_size = 10;
                    while (stream->has_content()) {
                        std::vector<char> chunk(chunk_size);
                        hestia::WriteableBufferView write_buffer(chunk);
                        const auto result = stream->read(write_buffer);
                        REQUIRE(result.ok());
                        if (result.m_num_transferred < chunk_size) {
                            chunk.resize(result.m_num_transferred);
                        }
                        hestia::ReadableBufferView read_buffer(chunk);
                        REQUIRE(m_working_context->write_to_stream(read_buffer)
                                    .ok());
                    }
                    (void)m_working_context->clear_stream();
                    m_web_app->on_event(
                        m_working_context.get(), hestia::HttpEvent::EOM);
                }
            }
        }
        else {
            m_web_app->on_event(
                m_working_context.get(), hestia::HttpEvent::HEADERS);

            if (m_working_context->get_response()->get_completion_status()
                != hestia::HttpResponse::CompletionStatus::FINISHED) {
                m_web_app->on_event(
                    m_working_context.get(), hestia::HttpEvent::EOM);
            }
        }
        completion_func(std::make_unique<hestia::HttpResponse>(
            *m_working_context->get_response()));
    }

    std::string m_user_name{"AKIAIOSFODNN7EXAMPLE"};
    std::string m_domain;
    bool m_should_apply_headers{true};
    std::unique_ptr<DistributedHsmServiceTestWrapper> m_fixture;
    std::unique_ptr<hestia::HestiaS3WebApp> m_web_app;
    std::unique_ptr<hestia::RequestContext> m_working_context;
};
}  // namespace hestia::mock
