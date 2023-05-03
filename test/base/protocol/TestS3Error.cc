#include <catch2/catch_all.hpp>

#include "S3Error.h"

#include <sstream>

TEST_CASE("Test S3Error", "[s3]")
{
    hestia::S3Error error(
        hestia::S3Error::Code::_400_AUTHORIZATION_HEADER_MALFORMED, "mybucket");

    std::stringstream sstr;
    sstr << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    sstr << "<Error>\n";
    sstr << "<Code>AuthorizationHeaderMalformed</Code>\n";
    sstr
        << "<Message>The authorization header you provided is invalid.</Message>\n";
    sstr << "<Resource>/mybucket</Resource>\n";
    sstr << "</Error>";

    std::stringstream error_sstr;
    error_sstr << error;
    REQUIRE(sstr.str() == error_sstr.str());
}