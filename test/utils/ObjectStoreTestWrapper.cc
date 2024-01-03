#include "ObjectStoreTestWrapper.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Logger.h"
#include "TestContext.h"

#include <catch2/catch_all.hpp>

#include <iostream>

#include <future>

ObjectStoreTestWrapper::ObjectStoreTestWrapper(
    hestia::ObjectStoreClient::Ptr client) :
    m_client_owner(std::move(client))
{
    m_client = m_client_owner.get();
}

ObjectStoreTestWrapper::ObjectStoreTestWrapper(const std::string& plugin_name)
{
    m_client_plugin_owner =
        TestContext::get().get_object_store_plugin(plugin_name);
    m_client = m_client_plugin_owner->get_client();
}

ObjectStoreTestWrapper::Ptr ObjectStoreTestWrapper::create(
    hestia::ObjectStoreClient::Ptr client)
{
    return std::make_unique<ObjectStoreTestWrapper>(std::move(client));
}

ObjectStoreTestWrapper::Ptr ObjectStoreTestWrapper::create(
    const std::string& plugin_name)
{
    return std::make_unique<ObjectStoreTestWrapper>(plugin_name);
}

void ObjectStoreTestWrapper::put(
    const hestia::StorageObject& obj, const std::string& content, bool flush)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    if (content.empty()) {
        hestia::ObjectStoreContext ctx(
            {obj, hestia::ObjectStoreRequestMethod::PUT}, completion_cb);
        m_client->make_request(ctx);
    }
    else {
        hestia::Stream stream;

        hestia::ReadableBufferView read_buffer(content);
        stream.set_source(hestia::InMemoryStreamSource::create(read_buffer));

        hestia::ObjectStoreContext ctx(
            {obj, hestia::ObjectStoreRequestMethod::PUT}, completion_cb,
            nullptr, &stream);
        m_client->make_request(ctx);
        if (flush) {
            stream.flush();
        }
    }
    const auto response = response_future.get();
    REQUIRE(response->ok());
}

void ObjectStoreTestWrapper::get(
    hestia::StorageObject& obj,
    std::string& buffer,
    std::size_t size,
    bool flush)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::Stream stream;
    std::vector<char> returned_buffer(size);
    hestia::WriteableBufferView write_buffer(returned_buffer);
    stream.set_sink(hestia::InMemoryStreamSink::create(write_buffer));

    hestia::ObjectStoreContext ctx(
        {obj, hestia::ObjectStoreRequestMethod::GET}, completion_cb, nullptr,
        &stream);

    m_client->make_request(ctx);

    if (flush) {
        stream.flush();
    }

    const auto response = response_future.get();
    REQUIRE(response->ok());

    buffer = std::string(returned_buffer.begin(), returned_buffer.end());
    obj    = response->object();
}

void ObjectStoreTestWrapper::get(hestia::StorageObject& obj)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::ObjectStoreContext ctx(
        {obj, hestia::ObjectStoreRequestMethod::GET}, completion_cb);

    m_client->make_request(ctx);
    const auto response = response_future.get();
    REQUIRE(response->ok());
    obj = response->object();
}

void ObjectStoreTestWrapper::exists(
    const hestia::StorageObject& obj, bool should_exist)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::ObjectStoreContext ctx(
        {obj, hestia::ObjectStoreRequestMethod::EXISTS}, completion_cb);
    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());

    REQUIRE(response->object_found() == should_exist);
}

void ObjectStoreTestWrapper::list(
    const hestia::KeyValuePair& query,
    std::vector<hestia::StorageObject>& result)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::ObjectStoreContext ctx({query}, completion_cb);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
    result = response->objects();
}

void ObjectStoreTestWrapper::remove(const hestia::StorageObject& obj)
{
    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::ObjectStoreContext ctx(
        {obj, hestia::ObjectStoreRequestMethod::REMOVE}, completion_cb);
    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
}