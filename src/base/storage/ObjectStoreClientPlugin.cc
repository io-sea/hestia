#include "ObjectStoreClientPlugin.h"

#include "Logger.h"

#include <stdexcept>

namespace hestia {

ObjectStoreClientPlugin::ObjectStoreClientPlugin(
    const PluginHandle* plugin_handle, ObjectStoreClient* client) :
    PluginResource(plugin_handle), m_client(client)
{
}

ObjectStoreClientPlugin::~ObjectStoreClientPlugin()
{
    m_plugin_handle->destroy_resource(this);
}

ObjectStoreClient* ObjectStoreClientPlugin::get_client() const
{
    return m_client;
}

ObjectStorePluginHandle::ObjectStorePluginHandle(
    void* raw_handle, const std::string& name) :
    PluginHandle(raw_handle, name)
{
}

void ObjectStorePluginHandle::destroy_resource(PluginResource* resource) const
{
    LOG_INFO("Destroying resource from: " << m_name);
    using func_t      = void(ObjectStoreClient*);
    auto destroy_func = reinterpret_cast<func_t*>(m_destroy_func);
    if (destroy_func == nullptr) {
        throw std::runtime_error("Function to destory resource is invalid");
    }

    if (auto object_resource =
            dynamic_cast<ObjectStoreClientPlugin*>(resource)) {
        destroy_func(object_resource->get_client());
    }
    else {
        LOG_ERROR("Failed to cast to ObjectStoreClientPlugin");
    }
}

std::unique_ptr<PluginResource>
ObjectStorePluginHandle::load_resource_with_factory_func() const
{
    using func_t     = ObjectStoreClient*();
    auto create_func = reinterpret_cast<func_t*>(m_create_func);
    auto client      = create_func();
    if (client == nullptr) {
        LOG_ERROR("Got invalid client from factory func.");
    }

    auto client_plugin =
        std::make_unique<ObjectStoreClientPlugin>(this, client);
    return client_plugin;
}

ObjectStorePluginFactory::ObjectStorePluginFactory(const std::string& name) :
    PluginFactory(name)
{
}

std::unique_ptr<PluginHandle> ObjectStorePluginFactory::create_handle(
    void* raw_handle) const
{
    return std::make_unique<ObjectStorePluginHandle>(raw_handle, m_name);
}

}  // namespace hestia