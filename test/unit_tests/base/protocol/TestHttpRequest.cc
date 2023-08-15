#include <catch2/catch_all.hpp>

#include "HttpRequest.h"

TEST_CASE("Test HttpRequest", "[protocol]")
{
    const std::string msg =
        "PUT /my_path?list-type=2&encoding-type=url HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length: 9\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "key=value";

    hestia::HttpRequest request;
    request.on_chunk(msg);

    REQUIRE(request.get_method_as_string() == "PUT");
    REQUIRE(request.get_header().get_content_length() == "9");
    REQUIRE(request.body() == "key=value");
    REQUIRE(request.get_path() == "/my_path");
    REQUIRE(request.get_queries().get_item("list-type") == "2");
    REQUIRE(request.get_queries().get_item("encoding-type") == "url");
}


TEST_CASE("Test HttpRequest - split headers", "[protocol]")
{
    const std::string msg0 =
        "PUT /my_path?list-type=2&encoding-type=url HTTP/1.1\r\n"
        "Host: localhost:8000\r\n"
        "Connection: keep-alive\r\n"
        "Content-Length:";

    const std::string msg1 =
        " 9\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "\r\n"
        "key=value";

    hestia::HttpRequest request;
    request.on_chunk(msg0);
    request.on_chunk(msg1);

    REQUIRE(request.get_method_as_string() == "PUT");
    REQUIRE(request.get_header().get_content_length() == "9");
    REQUIRE(request.body() == "key=value");
    REQUIRE(request.get_path() == "/my_path");
    REQUIRE(request.get_queries().get_item("list-type") == "2");
    REQUIRE(request.get_queries().get_item("encoding-type") == "url");
}
