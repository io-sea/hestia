#include <catch2/catch_all.hpp>

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "MockHttpClient.h"
#include "S3Client.h"
#include "S3ObjectStoreClient.h"

#include "ObjectStoreTestWrapper.h"
#include <iostream>

class S3ObjectStoreClientTestFixture {
  public:
    S3ObjectStoreClientTestFixture()
    {
        m_http_client = std::make_unique<hestia::mock::MockHttpClientForS3>("");
        m_http_client->m_should_apply_headers = false;
        m_s3_client = std::make_unique<hestia::S3Client>(m_http_client.get());

        auto object_store_client =
            hestia::S3ObjectStoreClient::create(m_s3_client.get());
        m_object_store =
            ObjectStoreTestWrapper::create(std::move(object_store_client));
    }

    std::unique_ptr<hestia::mock::MockHttpClientForS3> m_http_client;
    hestia::S3Client::Ptr m_s3_client;
    ObjectStoreTestWrapper::Ptr m_object_store;
};

TEST_CASE_METHOD(S3ObjectStoreClientTestFixture, "S3 client put", "[s3]")
{
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::StorageObject obj("0000");
    obj.set_size(content.size());
    obj.get_metadata_as_writeable().set_item(
        "hestia-user_id", m_http_client->m_user_name);
    obj.get_metadata_as_writeable().set_item(
        "hestia-user_token", m_http_client->m_fixture->get_user_token());

    m_object_store->put(obj, content, false);

    std::string returned_content;
    m_object_store->get(obj, returned_content, content.size(), false);
    REQUIRE(returned_content == content);
}