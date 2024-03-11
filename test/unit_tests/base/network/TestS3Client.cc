#include <catch2/catch_all.hpp>

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "MockHttpClient.h"
#include "S3Client.h"

#include <future>

class S3ClientTestFixture {
  public:
    S3ClientTestFixture()
    {
        std::string domain = "127.0.0.1:8000";
        m_http_client =
            std::make_unique<hestia::mock::MockHttpClientForS3>(domain);

        m_http_client->m_should_apply_headers = false;
        m_user_context.m_user_id              = m_http_client->m_user_name;
        m_user_context.m_user_secret_key =
            m_http_client->m_fixture->get_user_token();

        m_client = std::make_unique<hestia::S3Client>(m_http_client.get());
    }

    void put_object(
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

    void get_object(
        const hestia::S3Object& object,
        const hestia::S3Bucket& bucket,
        const hestia::S3Request& req,
        std::string& data,
        hestia::Map& metadata,
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
        metadata = response->m_content.m_metadata;
        data     = std::string(buffer.begin(), buffer.end());
    }

    std::unique_ptr<hestia::mock::MockHttpClientForS3> m_http_client;
    hestia::S3Client::Ptr m_client;

    hestia::S3UserContext m_user_context;
};

TEST_CASE_METHOD(S3ClientTestFixture, "Test S3 Client", "[protocol]")
{
    hestia::S3Request request(m_user_context, "127.0.0.1:8000");
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
        m_user_context, "127.0.0.1:8000");
    list_object_request.m_s3_request = request;
    list_object_request.build_query();

    const auto object_list_response =
        m_client->list_objects(bucket0, list_object_request);
    REQUIRE(object_list_response->ok());
    REQUIRE(object_list_response->get_num_objects() == 0);

    hestia::S3Object object0("object0");
    hestia::Map input_metadata;
    input_metadata.set_item("my_key", "my_value");
    object0.m_metadata = input_metadata;

    std::string obj_data = "The quick brown fox jumps over the lazy dog.";
    put_object(object0, bucket0, request, obj_data);

    const auto object_list_response1 =
        m_client->list_objects(bucket0, list_object_request);
    REQUIRE(object_list_response1->ok());
    REQUIRE(object_list_response1->get_num_objects() == 1);

    std::string returned_data;
    hestia::Map returned_metadata;
    get_object(
        object0, bucket0, request, returned_data, returned_metadata,
        obj_data.size());
    REQUIRE(returned_data == obj_data);
    REQUIRE(returned_metadata.get_item("my_key") == "my_value");
}