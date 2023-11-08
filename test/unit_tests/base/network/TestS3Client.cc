#include <catch2/catch_all.hpp>

#include "HttpClient.h"
#include "S3Client.h"

class MockHttpClientForS3 : public hestia::HttpClient {
  public:
    MockHttpClientForS3() {}

    void make_request(
        const hestia::HttpRequest& request,
        hestia::HttpClient::completionFunc completion_func,
        hestia::Stream*,
        std::size_t,
        hestia::HttpClient::progressFunc) override
    {
        m_last_request = request;
        completion_func(
            std::make_unique<hestia::HttpResponse>(m_next_response));
    }

    hestia::HttpRequest m_last_request;
    hestia::HttpResponse m_next_response;
};

TEST_CASE("Test S3 Client", "[protocol]")
{
    MockHttpClientForS3 http_client;
    hestia::S3Client s3_client(&http_client);
}