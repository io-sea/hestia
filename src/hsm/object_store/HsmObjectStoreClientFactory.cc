#include "HsmObjectStoreClientFactory.h"

#include "FileHsmObjectStoreClient.h"
#include "FileObjectStoreClient.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryObjectStoreClient.h"

#include "Logger.h"

namespace hestia {

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::path>& search_paths) :
    m_search_paths(search_paths)
{
}

bool ObjectStorePluginHandler::has_plugin(
    const HsmObjectStoreClientBackend& client_spec)
{
    if (client_spec.is_hsm()) {
        return bool(get_hsm_object_store_plugin(client_spec));
    }
    else {
        return bool(get_object_store_plugin(client_spec));
    }
}

std::unique_ptr<ObjectStoreClientPlugin>
ObjectStorePluginHandler::get_object_store_plugin(
    const HsmObjectStoreClientBackend& client_spec)
{
    const auto plugin_path = client_spec.get_plugin_path();
    LOG_INFO("Looking for plugin: " << plugin_path);

    ObjectStorePluginFactory plugin_factory(plugin_path);
    auto plugin =
        m_plugin_loader.load_plugin_resource(m_search_paths, plugin_factory);

    auto raw_plugin = plugin.get();
    if (auto typed_plugin =
            dynamic_cast<ObjectStoreClientPlugin*>(raw_plugin)) {
        auto cast_ptr = std::unique_ptr<ObjectStoreClientPlugin>(typed_plugin);
        plugin.release();
        return cast_ptr;
    }
    else {
        LOG_ERROR("Failed to cast resource to Object Store type");
        return nullptr;
    }
}

std::unique_ptr<HsmObjectStoreClientPlugin>
ObjectStorePluginHandler::get_hsm_object_store_plugin(
    const HsmObjectStoreClientBackend& client_spec)
{
    const auto plugin_path = client_spec.get_plugin_path();
    LOG_INFO("Looking for plugin: " << plugin_path);

    HsmObjectStorePluginFactory plugin_factory(plugin_path);
    auto plugin_resource =
        m_plugin_loader.load_plugin_resource(m_search_paths, plugin_factory);

    auto raw_plugin_resource = plugin_resource.get();
    if (auto typed_plugin =
            dynamic_cast<HsmObjectStoreClientPlugin*>(raw_plugin_resource)) {
        auto cast_ptr =
            std::unique_ptr<HsmObjectStoreClientPlugin>(typed_plugin);
        plugin_resource.release();
        return cast_ptr;
    }
    else {
        LOG_ERROR("Failed to cast resource to HSM Object Store type");
        return nullptr;
    }
}

HsmObjectStoreClientFactory::HsmObjectStoreClientFactory(
    ObjectStorePluginHandler::Ptr plugin_handler) :
    m_plugin_handler(std::move(plugin_handler))
{
}

bool HsmObjectStoreClientFactory::is_client_type_available(
    const HsmObjectStoreClientBackend& client_spec) const
{
    if (client_spec.is_built_in()) {
        return true;
    }
    return m_plugin_handler->has_plugin(client_spec);
}

ObjectStoreClient::Ptr HsmObjectStoreClientFactory::get_client(
    const HsmObjectStoreClientBackend& client_spec) const
{
    if (client_spec.is_built_in()) {
        if (client_spec.get_identifier()
            == FileHsmObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileHsmObjectStoreClient");
            return FileHsmObjectStoreClient::create();
        }
        else if (
            client_spec.get_identifier()
            == hestia::FileObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileObjectStoreClient");
            return hestia::FileObjectStoreClient::create();
        }
        else if (
            client_spec.get_identifier()
            == hestia::InMemoryHsmObjectStoreClient::
                get_registry_identifier()) {
            LOG_INFO("Setting up InMemoryHsmObjectStoreClient");
            return hestia::InMemoryHsmObjectStoreClient::create();
        }
        else if (
            client_spec.get_identifier()
            == hestia::InMemoryObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up InMemoryObjectStoreClient");
            return hestia::InMemoryObjectStoreClient::create();
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());

    return nullptr;
}

ObjectStoreClientPlugin::Ptr
HsmObjectStoreClientFactory::get_client_from_plugin(
    const HsmObjectStoreClientBackend& client_spec) const
{
    if (client_spec.is_plugin() || client_spec.is_mock()) {
        if (!client_spec.is_hsm()) {
            return m_plugin_handler->get_object_store_plugin(client_spec);
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());

    return nullptr;
}

HsmObjectStoreClientPlugin::Ptr
HsmObjectStoreClientFactory::get_hsm_client_from_plugin(
    const HsmObjectStoreClientBackend& client_spec) const
{
    if (client_spec.is_plugin() || client_spec.is_mock()) {
        if (client_spec.is_hsm()) {
            return m_plugin_handler->get_hsm_object_store_plugin(client_spec);
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());

    return nullptr;
}

}  // namespace hestia
