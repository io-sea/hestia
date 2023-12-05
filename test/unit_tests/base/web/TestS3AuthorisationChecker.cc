#include <catch2/catch_all.hpp>

#include <fstream>
#include <iostream>

#include "S3AuthorisationChecker.h"

#include "HttpRequest.h"
#include "InMemoryKeyValueStoreClient.h"
#include "UserService.h"
#include "UserTokenGenerator.h"

class MockTokenGenerator : public hestia::UserTokenGenerator {
  public:
    std::string generate(const std::string& key = {}) const override
    {
        (void)key;
        return m_token;
    }

    std::string m_token{"wJalrXUtnFEMI/K7MDENG/bPxRfiCYEXAMPLEKEY"};
};

class TestS3AuthorizationFixture {
  public:
    TestS3AuthorizationFixture()
    {
        m_original_assert_state = hestia::Logger::get_instance()
                                      .get_modifiable_context()
                                      .m_config.should_assert();
        hestia::Logger::get_instance()
            .get_modifiable_context()
            .m_config.set_should_assert_on_error(false);

        m_user.set_name("AKIAIOSFODNN7EXAMPLE");

        auto token_generator = std::make_unique<MockTokenGenerator>();
        m_token_generator    = token_generator.get();

        hestia::KeyValueStoreCrudServiceBackend backend(&m_kv_store_client);
        m_user_service = hestia::UserService::create(
            {}, &backend, nullptr, nullptr, std::move(token_generator));

        add_user();
    }

    ~TestS3AuthorizationFixture()
    {
        hestia::Logger::get_instance()
            .get_modifiable_context()
            .m_config.set_should_assert_on_error(m_original_assert_state);
    }

    void add_user()
    {
        auto response = m_user_service->register_user(m_user.name(), "");
        REQUIRE(response->ok());
        m_user = *response->get_item_as<hestia::User>();
    }

    void remove_user()
    {
        REQUIRE(m_user_service
                    ->make_request(hestia::CrudRequest{
                        hestia::CrudMethod::REMOVE,
                        {hestia::CrudIdentifier(m_user.id())},
                        {}})
                    ->ok());
    }

    hestia::InMemoryKeyValueStoreClient m_kv_store_client;
    MockTokenGenerator* m_token_generator{nullptr};
    std::unique_ptr<hestia::UserService> m_user_service;
    hestia::User m_user;
    bool m_original_assert_state{false};
};

std::string empty_body_hash =
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
std::string bad_empty_body_hash =
    "e3b0544298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";

TEST_CASE_METHOD(
    TestS3AuthorizationFixture,
    "Test S3 authorization - object",
    "[authorisation]")
{
    const std::string domain      = "s3.amazonaws.com";
    const std::string bucket_name = "examplebucket";

    hestia::HttpRequest request("/test.txt", hestia::HttpRequest::Method::GET);
    request.get_header().set_item("Host", bucket_name + "." + domain);
    request.get_header().set_item("x-amz-date", "20130524T000000Z");

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
                const auto response = hestia::S3AuthorisationChecker::authorise(
                    *m_user_service, request, domain);
                REQUIRE(
                    response.m_status
                    != hestia::S3AuthorisationChecker::Status::FAILED);
            }
        }


        WHEN("The request is NOT valid")
        {
            request.get_header().set_item(
                "x-amz-content-sha256", bad_empty_body_hash);

            THEN("The authorisation is not valid")
            {
                const auto response = hestia::S3AuthorisationChecker::authorise(
                    *m_user_service, request, domain);
                REQUIRE(
                    response.m_status
                    == hestia::S3AuthorisationChecker::Status::FAILED);
            }
        }

        WHEN("The signing user does not exist")
        {
            remove_user();
            request.get_header().set_item(
                "x-amz-content-sha256", empty_body_hash);

            THEN("The authorisation is not valid")
            {
                const auto response = hestia::S3AuthorisationChecker::authorise(
                    *m_user_service, request, domain);
                REQUIRE(
                    response.m_status
                    == hestia::S3AuthorisationChecker::Status::FAILED);
            }
        }
    }
}

TEST_CASE_METHOD(
    TestS3AuthorizationFixture, "S3 authorization - bucket", "[authorisation]")
{
    const std::string domain      = "s3.amazonaws.com";
    const std::string bucket_name = "examplebucket";

    hestia::HttpRequest request("/", hestia::HttpRequest::Method::GET);
    request.get_header().set_item("Host", bucket_name + "." + domain);
    request.get_header().set_item("x-amz-date", "20130524T000000Z");
    request.get_header().set_item("x-amz-content-sha256", empty_body_hash);

    const std::string credential =
        "AKIAIOSFODNN7EXAMPLE/20130524/us-east-1/s3/aws4_request";
    const std::string headers = "host;x-amz-content-sha256;x-amz-date";

    WHEN("The GET request contains one empty valued query")
    {
        hestia::Map queries;
        queries.set_item("lifecycle", "");
        request.set_queries(queries);
        const std::string sig =
            "fea454ca298b7da1c68078a5d1bdbfbbe0d65c699e0f91ac7a200a0136783543";
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authorisation is valid")
        {
            const auto response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                != hestia::S3AuthorisationChecker::Status::FAILED);
        }
    }

    WHEN("The GET request contains two queries")
    {
        hestia::Map queries;
        queries.set_item("max-keys", "2");
        queries.set_item("prefix", "J");
        request.set_queries(queries);

        const std::string sig =
            "34b48302e7b5fa45bde8084f4b7868a86f0a534bc59db6670ed5711ef69dc6f7";
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        THEN("The authorisation is valid")
        {
            const auto response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                != hestia::S3AuthorisationChecker::Status::FAILED);
        }
    }
}

TEST_CASE_METHOD(
    TestS3AuthorizationFixture,
    "S3 authorization - put object",
    "[authorisation]")
{
    const std::string domain      = "s3.amazonaws.com";
    const std::string bucket_name = "examplebucket";

    hestia::HttpRequest request(
        "/test$file.text", hestia::HttpRequest::Method::PUT);
    request.get_header().set_item("Host", bucket_name + "." + domain);
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

    WHEN("The authorization header is not valid")
    {
        request.get_header().set_item(
            "Authorization",
            "AWS4-HMAC-SHA256 SignedHeaders=" + headers + ",Signature=" + sig);

        THEN("The authorisation will fail before payload is added")
        {
            const auto response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                == hestia::S3AuthorisationChecker::Status::FAILED);
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
            auto result = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                result.m_status
                == hestia::S3AuthorisationChecker::Status::WAITING_FOR_PAYLOAD);

            THEN("After adding the payload, the authorisation is valid")
            {
                request.body() = "Welcome to Amazon S3.";
                result         = hestia::S3AuthorisationChecker::authorise(
                    *m_user_service, request, domain);
                REQUIRE(
                    result.m_status
                    == hestia::S3AuthorisationChecker::Status::VALID);
            }
        }
    }

    WHEN("The payload is NOT valid")
    {
        request.get_header().set_item(
            "Authorization", "AWS4-HMAC-SHA256 Credential=" + credential
                                 + ",SignedHeaders=" + headers
                                 + ",Signature=" + sig);

        auto result = hestia::S3AuthorisationChecker::authorise(
            *m_user_service, request, domain);
        REQUIRE(
            result.m_status
            == hestia::S3AuthorisationChecker::Status::WAITING_FOR_PAYLOAD);

        request.body() = "Goodbye from Amazon S3.";
        THEN("The authorisation is not valid")
        {
            result = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                result.m_status
                == hestia::S3AuthorisationChecker::Status::FAILED);
        }
    }
}

TEST_CASE_METHOD(
    TestS3AuthorizationFixture,
    "S3 authorization - unsigned-payload",
    "[.authorisation]")
{
    std::string domain = "192.168.2.117:11000";

    hestia::HttpRequest request(
        "lustre_hsm_4/0000000200000401_00000004_00000000.0",
        hestia::HttpRequest::Method::PUT);
    request.get_header().set_item("Content-Type", "application/x-lz4");
    request.get_header().set_item("Host", domain);
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

    m_token_generator->m_token = "SECRET_KEY";
    remove_user();

    m_user = hestia::User();
    m_user.set_name("OPEN_KEY");
    add_user();

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
            auto response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                != hestia::S3AuthorisationChecker::Status::FAILED);

            request.body() = "Text that should not matter";

            response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                != hestia::S3AuthorisationChecker::Status::FAILED);
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
            const auto response = hestia::S3AuthorisationChecker::authorise(
                *m_user_service, request, domain);
            REQUIRE(
                response.m_status
                == hestia::S3AuthorisationChecker::Status::FAILED);
        }
    }
}