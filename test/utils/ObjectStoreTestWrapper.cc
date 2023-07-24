#include "ObjectStoreTestWrapper.h"

#include "Logger.h"
#include "TestContext.h"

#include <catch2/catch_all.hpp>

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
    const hestia::StorageObject& obj, hestia::Stream* stream)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::PUT);
    REQUIRE(m_client->make_request(request, stream)->ok());
}

void ObjectStoreTestWrapper::get(
    hestia::StorageObject& obj, hestia::Stream* stream)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::GET);
    auto response = m_client->make_request(request, stream);
    REQUIRE(response->ok());
    obj = response->object();
}

void ObjectStoreTestWrapper::exists(
    const hestia::StorageObject& obj, bool should_exist)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::EXISTS);
    const auto response = m_client->make_request(request);
    REQUIRE(response->ok());
    REQUIRE(response->object_found() == should_exist);
}

void ObjectStoreTestWrapper::list(
    const hestia::KeyValuePair& query,
    std::vector<hestia::StorageObject>& result)
{
    hestia::ObjectStoreRequest request(query);
    const auto response = m_client->make_request(request);
    REQUIRE(response->ok());
    result = response->objects();
}

void ObjectStoreTestWrapper::remove(const hestia::StorageObject& obj)
{
    hestia::ObjectStoreRequest request(
        obj, hestia::ObjectStoreRequestMethod::REMOVE);
    REQUIRE(m_client->make_request(request)->ok());
}