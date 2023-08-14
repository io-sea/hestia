#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "FileHsmObjectStoreClient.h"
#include "FileKeyValueStoreClient.h"

#include "HestiaS3WebApp.h"
#include "HsmService.h"
#include "UserService.h"

#include "DistributedHsmServiceTestWrapper.h"
#include "ObjectStoreTestWrapper.h"

#include "TestUtils.h"

#include <iostream>

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("s3_plugin")
    {
        m_hsm_service_wrapper =
            std::make_unique<DistributedHsmServiceTestWrapper>();

        hestia::KeyValueStoreCrudServiceBackend crud_backend(
            &m_hsm_service_wrapper->m_kv_store_client);

        m_user_service = hestia::UserService::create({}, &crud_backend);

        hestia::HestiaS3WebAppConfig web_app_config;
        m_web_app = std::make_unique<hestia::HestiaS3WebApp>(
            web_app_config, m_hsm_service_wrapper->m_dist_hsm_service.get(),
            m_user_service.get());

        hestia::Server::Config server_config;
        m_server = std::make_unique<hestia::BasicHttpServer>(
            server_config, m_web_app.get());

        m_server->initialize();
        m_server->start();
        m_server->wait_until_bound();

        m_client->initialize("0", {}, {});
    }

    std::unique_ptr<DistributedHsmServiceTestWrapper> m_hsm_service_wrapper;
    std::unique_ptr<hestia::UserService> m_user_service;

    std::unique_ptr<hestia::HestiaS3WebApp> m_web_app;
    std::unique_ptr<hestia::BasicHttpServer> m_server;

    std::string m_base_url = "127.0.0.1:8000/";
};

TEST_CASE_METHOD(S3ClientTestFixture, "Test Hestia S3 Web App", "[s3]")
{
    const auto id = "0000";
    hestia::StorageObject obj(id);
    obj.get_metadata_as_writeable().set_item("key", "0000");
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::ReadableBufferView read_buffer(content);
    auto source = hestia::InMemoryStreamSource::create(read_buffer);

    hestia::Stream stream;
    stream.set_source(std::move(source));

    return;
    put(obj, &stream);
    REQUIRE(stream.reset().ok());
    return;

    /*
    std::vector<char> returned_buffer(content.length());
    hestia::WriteableBufferView write_buffer(returned_buffer);

    auto sink = hestia::InMemoryStreamSink::create(write_buffer);
    stream.set_sink(std::move(sink));

    hestia::ObjectStoreRequest memory_store_req(
        obj, hestia::ObjectStoreRequestMethod::GET);
    REQUIRE(object_store->make_request(memory_store_req, &stream)->ok());

    // get(obj, &stream);
    REQUIRE(stream.reset().ok());

    std::string recontstructed_content(
        returned_buffer.begin(), returned_buffer.end());
    REQUIRE(recontstructed_content == content);
    */
}