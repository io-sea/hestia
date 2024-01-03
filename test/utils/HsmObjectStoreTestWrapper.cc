#include "HsmObjectStoreTestWrapper.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Logger.h"
#include "TestContext.h"

#include <future>

#include <catch2/catch_all.hpp>

HsmObjectStoreTestWrapper::HsmObjectStoreTestWrapper(
    hestia::HsmObjectStoreClient::Ptr client) :
    m_client_owner(std::move(client))
{
    m_client = m_client_owner.get();
}

HsmObjectStoreTestWrapper::HsmObjectStoreTestWrapper(
    const std::string& plugin_name)
{
    m_client_plugin_owner =
        TestContext::get().get_hsm_object_store_plugin(plugin_name);
    m_client = m_client_plugin_owner->get_client();
    m_client->set_tier_ids({"1", "2", "3", "4"});
}

HsmObjectStoreTestWrapper::Ptr HsmObjectStoreTestWrapper::create(
    hestia::HsmObjectStoreClient::Ptr client)
{
    return std::make_unique<HsmObjectStoreTestWrapper>(std::move(client));
}

HsmObjectStoreTestWrapper::Ptr HsmObjectStoreTestWrapper::create(
    const std::string& plugin_name)
{
    return std::make_unique<HsmObjectStoreTestWrapper>(plugin_name);
}

void HsmObjectStoreTestWrapper::put(
    const hestia::StorageObject& obj,
    const std::string& content,
    const std::string& tier)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::PUT);
    request.set_target_tier(tier);

    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    if (!content.empty()) {
        hestia::Stream stream;
        auto source = hestia::InMemoryStreamSource::create(
            hestia::ReadableBufferView(content));
        stream.set_source(std::move(source));

        hestia::HsmObjectStoreContext ctx(
            request, completion_cb, nullptr, &stream);
        m_client->make_request(ctx);

        REQUIRE(stream.flush().ok());
    }
    else {
        hestia::HsmObjectStoreContext ctx(
            request, completion_cb, nullptr, nullptr);
        m_client->make_request(ctx);
    }

    const auto response = response_future.get();
    REQUIRE(response->ok());
}

void HsmObjectStoreTestWrapper::get(hestia::StorageObject& obj)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::GET);

    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
    obj = response->object();
}

void HsmObjectStoreTestWrapper::get(
    hestia::StorageObject& obj,
    std::string& content,
    std::size_t content_length,
    const std::string& tier)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::GET);
    request.set_source_tier(tier);

    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::Stream stream;
    std::vector<char> returned_buffer(content_length);
    hestia::WriteableBufferView write_buffer(returned_buffer);
    auto sink = hestia::InMemoryStreamSink::create(write_buffer);
    stream.set_sink(std::move(sink));

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, &stream);

    m_client->make_request(ctx);

    REQUIRE(stream.flush().ok());

    auto response = response_future.get();
    REQUIRE(response->ok());

    content = std::string(returned_buffer.begin(), returned_buffer.end());
    obj     = response->object();
}

void HsmObjectStoreTestWrapper::exists(
    const hestia::StorageObject& obj, bool should_exist)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::EXISTS);

    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
    REQUIRE(response->object_found() == should_exist);
}

void HsmObjectStoreTestWrapper::copy(
    const hestia::StorageObject& obj,
    const std::string& source_tier,
    const std::string& target_tier)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::COPY);
    request.set_source_tier(source_tier);
    request.set_target_tier(target_tier);
    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
}

void HsmObjectStoreTestWrapper::move(
    const hestia::StorageObject& obj,
    const std::string& source_tier,
    const std::string& target_tier)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::MOVE);
    request.set_source_tier(source_tier);
    request.set_target_tier(target_tier);

    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
}

void HsmObjectStoreTestWrapper::list(
    const hestia::KeyValuePair& query,
    std::vector<hestia::StorageObject>& result)
{
    hestia::HsmObjectStoreRequest request(query);
    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
    result = response->objects();
}

void HsmObjectStoreTestWrapper::remove(const hestia::StorageObject& obj)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::REMOVE);
    std::promise<hestia::HsmObjectStoreResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb =
        [&response_promise](hestia::HsmObjectStoreResponse::Ptr response) {
            response_promise.set_value(std::move(response));
        };

    hestia::HsmObjectStoreContext ctx(request, completion_cb, nullptr, nullptr);

    m_client->make_request(ctx);
    auto response = response_future.get();
    REQUIRE(response->ok());
}