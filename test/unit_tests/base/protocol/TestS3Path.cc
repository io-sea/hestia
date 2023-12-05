#include <catch2/catch_all.hpp>

#include "S3Path.h"

TEST_CASE("Test S3 Path - virtual host version", "[s3]")
{
    WHEN("A bucket and object are given")
    {
        hestia::HttpRequest request(
            "/my_object", hestia::HttpRequest::Method::GET);
        request.get_header().set_item("Host", "container.123.com");

        hestia::S3Path path(request, "123.com");

        REQUIRE(path.m_object_key == "my_object");
        REQUIRE(path.m_bucket_name == "container");
    }

    WHEN("Just a bucket is given")
    {
        hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
        request.get_header().set_item("Host", "container.123.com");

        hestia::S3Path path(request, "123.com");

        REQUIRE(path.m_object_key.empty());
        REQUIRE(path.m_bucket_name == "container");
    }

    WHEN("Nothing is given")
    {
        hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request, "");

        REQUIRE(path.m_object_key.empty());
        REQUIRE(path.m_bucket_name.empty());
    }
}

TEST_CASE("Test S3 Path - path version", "[s3]")
{
    WHEN("The path is explicitly given in ctor")
    {
        hestia::HttpRequest request(
            "/my_bucket/my_object", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request.get_path());

        REQUIRE(path.m_object_key == "my_object");
        REQUIRE(path.m_bucket_name == "my_bucket");
    }

    WHEN("The path is explicitly given in the request")
    {
        hestia::HttpRequest request(
            "/my_bucket/my_object", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request, "");

        REQUIRE(path.m_object_key == "my_object");
        REQUIRE(path.m_bucket_name == "my_bucket");
    }

    WHEN("Just a bucket is given")
    {
        hestia::HttpRequest request(
            "/my_bucket", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request.get_path());

        REQUIRE(path.m_object_key.empty());
        REQUIRE(path.m_bucket_name == "my_bucket");
    }

    WHEN("Nothing is given")
    {
        hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request.get_path());

        REQUIRE(path.m_object_key.empty());
        REQUIRE(path.m_bucket_name.empty());
    }

    WHEN("A bucket and query are given")
    {
        hestia::HttpRequest request(
            "/my_bucket/?location", hestia::HttpRequest::Method::GET);
        hestia::S3Path path(request.get_path());

        REQUIRE(path.m_object_key.empty());
        REQUIRE(path.m_bucket_name == "my_bucket");
        REQUIRE(path.m_queries == "location");
    }
}