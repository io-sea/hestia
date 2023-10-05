#include <catch2/catch_all.hpp>

#include "HttpClient.h"
#include "S3Client.h"

class MockHttpClientForS3 : public hestia::HttpClient {
  public:
    MockHttpClientForS3() {}

    hestia::HttpResponse::Ptr make_request(
        const hestia::HttpRequest& request, hestia::Stream*) override
    {
        m_last_request = request;
        return std::make_unique<hestia::HttpResponse>(m_next_response);
    }

    hestia::HttpRequest m_last_request;
    hestia::HttpResponse m_next_response;
};

TEST_CASE("Test S3 Client", "[protocol]")
{
    MockHttpClientForS3 http_client;
    hestia::S3Client s3_client(&http_client);
}