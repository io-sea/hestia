#include <catch2/catch_all.hpp>

#include "HttpRequest.h"

TEST_CASE("Test HttpRequest", "[protocol]")
{
    const std::string msg = "PUT // HTTP/1.1\n"
                            "Host: localhost:8000\n"
                            "Connection: keep-alive\n"
                            "Content-Length: 9\n"
                            "Content-Type: application/x-www-form-urlencoded\n"
                            "\n"
                            "key=value";

    hestia::HttpRequest request(msg);
    REQUIRE(request.get_method_as_string() == "PUT");
    REQUIRE(request.get_header().get_content_length() == " 9");
    REQUIRE(request.body() == "key=value");
    REQUIRE(request.get_path() == "/");
}