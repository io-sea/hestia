#include <catch2/catch_all.hpp>

#include "HttpResponse.h"
#include "S3Error.h"
#include "S3Request.h"

#include <iostream>

TEST_CASE("Test S3Error", "[s3]")
{
    hestia::S3Request request;
    request.m_path        = hestia::S3Path("/my_bucket");
    request.m_tracking_id = "1234";

    hestia::S3Error error(
        hestia::S3Error::Code::_400_AUTHORIZATION_HEADER_MALFORMED, request);

    hestia::HttpResponse http_response;
    http_response.body() = error.to_string();

    hestia::S3Error reconstructed_error(http_response);
    REQUIRE(reconstructed_error.to_string() == error.to_string());
}