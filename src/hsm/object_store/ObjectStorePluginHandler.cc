#include "ObjectStorePluginHandler.h"

#include "Logger.h"

namespace hestia {

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::path>& search_paths) :
    m_search_paths(search_paths)
{
}

bool ObjectStorePluginHandler::has_plugin(const ObjectStoreBackend& client_spec)
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
    const ObjectStoreBackend& client_spec)
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
    const ObjectStoreBackend& client_spec)
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

}  // namespace hestia