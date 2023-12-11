#include <catch2/catch_all.hpp>

#include "CurlClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "S3Client.h"

#include <future>

class S3ClientRealBackendTestFixture {
  public:
    S3ClientRealBackendTestFixture()
    {
        m_user_context.m_user_id         = m_user_id;
        m_user_context.m_user_secret_key = m_user_pass;
        hestia::CurlClientConfig http_config;

        m_http_client = std::make_unique<hestia::CurlClient>(http_config);
        m_client      = std::make_unique<hestia::S3Client>(m_http_client.get());
    }

    void put_data(
        const hestia::S3Object& object,
        const hestia::S3Bucket& bucket,
        const hestia::S3Request& req,
        const std::string& data)
    {
        hestia::ReadableBufferView read_buffer(data);
        hestia::Stream stream;
        stream.set_source(hestia::InMemoryStreamSource::create(read_buffer));

        std::promise<hestia::S3Response::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::S3Response::Ptr response) {
                response_promise.set_value(std::move(response));
            };
        m_client->put_object(object, bucket, req, &stream, completion_cb);

        const auto response = response_future.get();
        REQUIRE(response->is_ok());
    }

    void get_data(
        const hestia::S3Object& object,
        const hestia::S3Bucket& bucket,
        const hestia::S3Request& req,
        std::string& data,
        std::size_t size)
    {
        std::vector<char> buffer(size);
        hestia::WriteableBufferView write_buffer(buffer);
        hestia::Stream stream;
        stream.set_sink(hestia::InMemoryStreamSink::create(write_buffer));

        std::promise<hestia::S3Response::Ptr> response_promise;
        auto response_future = response_promise.get_future();

        auto completion_cb =
            [&response_promise](hestia::S3Response::Ptr response) {
                response_promise.set_value(std::move(response));
            };
        m_client->get_object(object, bucket, req, &stream, completion_cb);

        const auto response = response_future.get();
        REQUIRE(response->is_ok());

        data = std::string(buffer.begin(), buffer.end());
    }

    std::unique_ptr<hestia::CurlClient> m_http_client;
    hestia::S3Client::Ptr m_client;
    std::string m_endpoint{"127.0.0.1:9000"};
    std::string m_user_id{"minioadmin"};
    std::string m_user_pass{"minioadmin"};
    hestia::S3UserContext m_user_context;
};

TEST_CASE_METHOD(
    S3ClientRealBackendTestFixture,
    "Test S3 Client Real Backend",
    "[.protocol]")
{
    // Run manually for now - needs clean minio server running on default ports.

    hestia::S3Request request(m_user_context, m_endpoint);
    request.m_endpoint       = m_endpoint;
    request.m_signed_headers = {"host", "x-amz-content-sha256", "x-amz-date"};

    const auto list_bucket_response = m_client->list_buckets(request);
    REQUIRE(list_bucket_response->ok());
    REQUIRE(list_bucket_response->get_num_buckets() == 0);

    hestia::S3Bucket bucket0("bucket0");
    const auto create_bucket_status = m_client->create_bucket(bucket0, request);
    REQUIRE(create_bucket_status.is_ok());

    const auto list_bucket_response1 = m_client->list_buckets(request);
    REQUIRE(list_bucket_response1->ok());
    REQUIRE(list_bucket_response1->get_num_buckets() == 1);

    hestia::S3ListObjectsRequest list_object_request(
        m_user_context, m_endpoint);
    list_object_request.m_s3_request = request;
    list_object_request.build_query();

    /* TODO: Some issue with signature calc remaining here.
    const auto object_list_response =
        m_client->list_objects(bucket0, list_object_request);
    REQUIRE(object_list_response->ok());
    REQUIRE(object_list_response->get_num_objects() == 0);
    */

    hestia::S3Object object0("object0");
    std::string obj_data = "The quick brown fox jumps over the lazy dog.";
    put_data(object0, bucket0, request, obj_data);

    /*
    const auto object_list_response1 =
        m_client->list_objects(bucket0, list_object_request);
    REQUIRE(object_list_response1->ok());
    REQUIRE(object_list_response1->get_num_objects() == 1);
    */

    std::string returned_data;
    get_data(object0, bucket0, request, returned_data, obj_data.size());
    REQUIRE(returned_data == obj_data);
}