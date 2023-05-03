#include <catch2/catch_all.hpp>

#include <fstream>

#include "S3AuthorisationSession.h"

#include "HttpRequest.h"
#include "UserService.h"

class MockUserService : public hestia::UserService {
  public:
    MockUserService() : hestia::UserService(nullptr) {}

    void fetch_token(hestia::User& user) const override
    {
        if (m_has_user) {
            user.m_token      = m_token;
            user.m_identifier = m_identifier;
        }
    }
    std::string m_identifier = "AKIAIOSFODNN7EXAMPLE";
    std::string m_token      = "wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY";
    bool m_has_user          = true;
};

std::string empty_body_hash =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
std::string bad_empty_body_hash =
    "e3b0544298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

TEST_CASE("S3 authorization - object", "[authorisation]")
{
    hestia::HttpRequest request("/test.txt", hestia::HttpRequest::Method::GET);
    request.get_header().set_item("Host", "examplebucket.s3.amazonaws.com");
    request.get_header().set_item("x-amz-date", "20130524T000000Z");

    MockUserService user_service;
    hestia::S3AuthorisationSession auth_session(&user_service);

    GIVEN("A GET request")
    {
        const std::string credential =
            "AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request";
        const std::string headers =
            "host;range;x-amz-content-sha256;x-amz-date";
        const std::string sig =
            "f0e8bdb87c964420e857bd35b5d6ed310bd44f0170aba48dd91039c6036bdb41";

        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);
        request.get_header().set_item("Range", "bytes=0-9");

        WHEN("The request is valid")
        {
            request.get_header().set_item(
                "x-amz-content-sha256", empty_body_hash);

            THEN("The authorisation is valid")
            {
                const auto result = auth_session.authorise(request);
                REQUIRE(result.is_valid());
            }
        }

        WHEN("The request is NOT valid")
        {
            request.get_header().set_item(
                "x-amz-content-sha256", bad_empty_body_hash);

            THEN("The authorisation is not valid")
            {
                const auto result = auth_session.authorise(request);
                REQUIRE_FALSE(result.is_valid());
            }
        }

        WHEN("The signing user does not exist")
        {
            user_service.m_has_user = false;
            request.get_header().set_item(
                "x-amz-content-sha256", empty_body_hash);

            THEN("The authorisation is not valid")
            {
                const auto result = auth_session.authorise(request);
                REQUIRE_FALSE(result.is_valid());
            }
        }
    }
}

TEST_CASE("S3 authorization - bucket", "[authorisation]")
{
    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    request.get_header().set_item("Host", "examplebucket.s3.amazonaws.com");
    request.get_header().set_item("x-amz-date", "20130524T000000Z");
    request.get_header().set_item("x-amz-content-sha256", empty_body_hash);

    const std::string credential =
        "AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request";
    const std::string headers = "host;x-amz-content-sha256;x-amz-date";

    MockUserService user_service;
    hestia::S3AuthorisationSession auth_session(&user_service);

    WHEN("The GET request contains one empty valued query")
    {
        request.set_queries("lifecycle");
        const std::string sig =
            "fea454ca298b7da1c68078a5d1bdbfbbe0d65c699e0f91ac7a200a0136783543";
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authorisation is valid")
        {
            const auto result = auth_session.authorise(request);
            REQUIRE(result.is_valid());
        }
    }

    WHEN("The GET request contains two queries")
    {
        request.set_queries("max-keys=2&prefix=J");
        const std::string sig =
            "34b48302e7b5fa45bde8084f4b7868a86f0a534bc59db6670ed5711ef69dc6f7";
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authorisation is valid")
        {
            const auto result = auth_session.authorise(request);
            REQUIRE(result.is_valid());
        }
    }
}

TEST_CASE("S3 authorization - put object", "[authorisation]")
{
    hestia::HttpRequest request(
        "/test$file.text", hestia::HttpRequest::Method::PUT);
    request.get_header().set_item("Host", "examplebucket.s3.amazonaws.com");
    request.get_header().set_item("x-amz-date", "20130524T000000Z");
    request.get_header().set_item("Date", "Fri, 24 May 2013 00:00:00 GMT");
    request.get_header().set_item("x-amz-storage-class", "REDUCED_REDUNDANCY");
    request.get_header().set_item(
        "x-amz-content-sha256",
        "44ce7dd67c959e0d3524ffac1771dfbba87d2b6b4b4e99e42034a8b803f8b072");
    request.get_header().set_item("Content-Length", "21");

    const std::string credential =
        "AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request";
    const std::string headers =
        "date;host;x-amz-content-sha256;x-amz-date;x-amz-storage-class";
    const std::string sig =
        "98ad721746da40c64f1a55b78f14c238d841ea1380cd77a1b5971af0ece108bd";

    MockUserService user_service;
    hestia::S3AuthorisationSession auth_session(&user_service);

    WHEN("The authorization header is not valid")
    {
        request.get_header().set_item(
            "Authorization",
            "AWS4-HMAC-SHA256 SignedHeaders=" + headers + ",Signature=" + sig);

        THEN("The authorisation will fail before payload is added")
        {
            const auto result = auth_session.authorise(request);
            REQUIRE_FALSE(result.is_valid());
        }
    }

    WHEN("The request is valid")
    {
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN(
            "After the first call to authorise() the status is 'waiting_for_payload'")
        {
            auto result = auth_session.authorise(request);
            REQUIRE(result.is_waiting_for_payload());

            THEN("After adding the payload, the authorisation is valid")
            {
                auth_session.add_chunk("Welcome to Amazon S3.");
                result = auth_session.authorise(request);
                REQUIRE(result.is_valid());
            }
        }
    }

    WHEN("The payload is NOT valid")
    {
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        auto result = auth_session.authorise(request);
        auth_session.add_chunk("Goodbye from Amazon S3.");
        THEN("The authorisation is not valid")
        {
            auto result = auth_session.authorise(request);
            REQUIRE_FALSE(result.is_valid());
        }
    }
}

TEST_CASE("S3 authorization - unsigned-payload", "[authorisation]")
{
    MockUserService user_service;
    hestia::S3AuthorisationSession auth_session(&user_service);

    hestia::HttpRequest request(
        "/lustre_hsm_4/0000000200000401_00000004_00000000.0",
        hestia::HttpRequest::Method::PUT);
    request.get_header().set_item("Content-Type", "application/x-lz4");
    request.get_header().set_item("Host", "192.168.2.117:11000");
    request.get_header().set_item("x-amz-content-sha256", "UNSIGNED-PAYLOAD");
    request.get_header().set_item("x-amz-date", "20200612T095534Z");
    request.get_header().set_item("x-amz-meta-chunksize", "104857600");
    request.get_header().set_item("x-amz-meta-gid", "0");
    request.get_header().set_item(
        "x-amz-meta-path", "/mnt/lustre/.lustre/fid/0x200000401:0x4:0x0");
    request.get_header().set_item("x-amz-meta-stripecount", "1");
    request.get_header().set_item("x-amz-meta-stripesize", "1048576");
    request.get_header().set_item("x-amz-meta-totallength", "50");
    request.get_header().set_item("x-amz-meta-uid", "0");
    request.get_header().set_item("Content-Length", "52");

    user_service.m_identifier = "OPEN_KEY";
    user_service.m_token      = "SECRET_KEY";
    const std::string credential =
        "OPEN_KEY/20200612/us-east-1/s3/aws4_request";
    const std::string sig =
        "81506ba791e192d332f39c8d6cc6031794de31bead366f55e75cff6cc3e41e8d";

    WHEN("The authentication header is valid")
    {
        const std::string headers =
            "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-meta-chunksize;x-amz-meta-gid;x-amz-meta-path;x-amz-meta-stripecount;x-amz-meta-stripesize;x-amz-meta-totallength;x-amz-meta-uid";

        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authentication is valid")
        {
            auto result = auth_session.authorise(request);
            REQUIRE(result.is_valid());

            auth_session.add_chunk("Text that should not matter");

            result = auth_session.authorise(request);
            REQUIRE(result.is_valid());
        }
    }

    WHEN("The authentication header is not valid")
    {
        // missing x-amz-meta-gid:
        const std::string headers =
            "content-type;host;x-amz-content-sha256;x-amz-date;x-amz-meta-chunksize;x-amz-meta-path;x-amz-meta-stripecount;x-amz-meta-stripesize;x-amz-meta-totallength;x-amz-meta-uid";

        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authorisation is not valid")
        {
            auto result = auth_session.authorise(request);
            REQUIRE_FALSE(result.is_valid());
        }
    }
}