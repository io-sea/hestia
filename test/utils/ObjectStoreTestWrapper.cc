#include "ObjectStoreTestWrapper.h"

#include "Logger.h"
#include "TestContext.h"

#include <catch2/catch_all.hpp>

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
    const hestia::StorageObject& obj,
    const std::string& content,
    std::size_t chunk_size)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::PUT);

    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    if (content.empty()) {
        m_client->make_request(request, completion_cb);
    }
    else {
        hestia::Stream stream;
        m_client->make_request(request, completion_cb, nullptr, &stream);
        if (chunk_size == 0) {
            REQUIRE(stream.write(content).ok());
        }
        else {
            std::vector<char> data_chars(content.begin(), content.end());
            std::size_t cursor = 0;
            while (cursor < content.size()) {
                auto chunk_end = cursor + chunk_size;
                if (chunk_end >= content.size()) {
                    chunk_end = content.size();
                }
                if (chunk_end == cursor) {
                    break;
                }
                hestia::ReadableBufferView read_buffer(
                    &data_chars[0] + cursor, chunk_end - cursor);
                REQUIRE(stream.write(read_buffer).ok());
                cursor = chunk_end;
            }
        }
        REQUIRE(stream.reset().ok());
    }
    const auto response = response_future.get();
    REQUIRE(response->ok());
}

void ObjectStoreTestWrapper::get(
    hestia::StorageObject& obj, std::string& buffer, std::size_t size)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::GET);

    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::Stream stream;
    m_client->make_request(request, completion_cb, nullptr, &stream);

    std::vector<char> returned_buffer(size);
    hestia::WriteableBufferView write_buffer(returned_buffer);
    REQUIRE(stream.read(write_buffer).ok());
    REQUIRE(stream.reset().ok());
    buffer = std::string(returned_buffer.begin(), returned_buffer.end());

    const auto response = response_future.get();
    REQUIRE(response->ok());
    obj = response->object();
}

void ObjectStoreTestWrapper::get(hestia::StorageObject& obj)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::GET);

    std::promise<hestia::ObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::ObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    m_client->make_request(request, completion_cb);
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

    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::EXISTS);
    m_client->make_request(request, completion_cb);
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

    hestia::ObjectStoreRequest request(query);
    m_client->make_request(request, completion_cb);
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

    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::REMOVE);
    m_client->make_request(request, completion_cb);
    auto response = response_future.get();
    REQUIRE(response->ok());
}