#include <catch2/catch_all.hpp>

#include "BasicHttpServer.h"
#include "InMemoryObjectStoreClient.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ObjectStoreTestWrapper.h"
#include "S3WebApp.h"

#include <iostream>

class S3ClientTestFixture : public ObjectStoreTestWrapper {
  public:
    S3ClientTestFixture() : ObjectStoreTestWrapper("s3_plugin") {}
};

TEST_CASE_METHOD(S3ClientTestFixture, "Test S3 client - integration", "[.][s3]")
{
    auto object_store = hestia::InMemoryObjectStoreClient::create();

    hestia::S3ServiceConfig service_config;
    service_config.m_object_store_client = object_store.get();
    auto s3_service = std::make_unique<hestia::S3Service>(service_config);

    hestia::S3WebAppConfig config;
    auto web_app = std::make_unique<hestia::S3WebApp>(
        object_store.get(), config, std::move(s3_service));

    hestia::Server::Config server_config;
    hestia::BasicHttpServer server(server_config, web_app.get());

    server.initialize();
    server.start();
    server.wait_until_bound();

    const auto url = "127.0.0.1:8000/";

    hestia::Dictionary client_config;
    client_config.set_map({{"host", url}});

    m_client->initialize(client_config);

    hestia::StorageObject obj("0000");
    const std::string content = "The quick brown fox jumps over the lazy dog.";

    hestia::ReadableBufferView read_buffer(content);
    auto source = hestia::InMemoryStreamSource::create(read_buffer);

    hestia::Stream stream;
    stream.set_source(std::move(source));

    put(obj, &stream);
    REQUIRE(stream.reset().ok());
    return;

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
}