#include "HsmObjectStoreTestWrapper.h"

#include "Logger.h"
#include "TestContext.h"

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
    const hestia::StorageObject& obj, hestia::Stream* stream)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::PUT);
    REQUIRE(m_client->make_request(request, stream)->ok());
}

void HsmObjectStoreTestWrapper::get(
    hestia::StorageObject& obj, hestia::Stream* stream)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::GET);
    auto repsonse = m_client->make_request(request, stream);
    REQUIRE(repsonse->ok());
    obj = repsonse->object();
}

void HsmObjectStoreTestWrapper::exists(
    const hestia::StorageObject& obj, bool should_exist)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::EXISTS);
    const auto response = m_client->make_request(request);
    REQUIRE(response->ok());
    REQUIRE(response->object_found() == should_exist);
}

void HsmObjectStoreTestWrapper::list(
    const hestia::Metadata::Query& query,
    std::vector<hestia::StorageObject>& result)
{
    hestia::HsmObjectStoreRequest request(query);
    const auto response = m_client->make_request(request);
    REQUIRE(response->ok());
    result = response->objects();
}

void HsmObjectStoreTestWrapper::remove(const hestia::StorageObject& obj)
{
    hestia::HsmObjectStoreRequest request(
        obj, hestia::HsmObjectStoreRequestMethod::REMOVE);
    REQUIRE(m_client->make_request(request)->ok());
}