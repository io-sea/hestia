#include "HsmObjectStoreClientPlugin.h"

#include <ostk/Logger.h>

#include <exception>

HsmObjectStoreClientPlugin::HsmObjectStoreClientPlugin(const ostk::PluginHandle* pluginHandle, HsmObjectStoreClient* client)
    : PluginResource(pluginHandle),
        mClient(client)
{
}

HsmObjectStoreClientPlugin::~HsmObjectStoreClientPlugin()
{
    mPluginHandle->destroyResource(this);
}

HsmObjectStoreClient* HsmObjectStoreClientPlugin::getClient() const
{
    return mClient;
}

HsmObjectStorePluginHandle::HsmObjectStorePluginHandle(void* rawHandle, const std::string& name)
    : ostk::PluginHandle(rawHandle, name)
{
}

void HsmObjectStorePluginHandle::destroyResource(ostk::PluginResource* resource) const
{
    LOG_INFO("Destroying resource from: " << mName);
    using func_t = void(HsmObjectStoreClient*);
    auto destroy_func = reinterpret_cast<func_t*>(mDestroyFunc);
    if (!destroy_func)
    {
        throw std::runtime_error("Function to destory resource is invalid");
    }

    if (auto object_resource = dynamic_cast<HsmObjectStoreClientPlugin*>(resource))
    {
        destroy_func(object_resource->getClient());
    }
    else
    {
        LOG_ERROR("Failed to cast to ObjectStoreClientPlugin");
    }
}

std::unique_ptr<ostk::PluginResource> HsmObjectStorePluginHandle::loadResourceWithFactoryFunc() const
{
    using func_t = HsmObjectStoreClient*(void);
    auto create_func = reinterpret_cast<func_t*>(mCreateFunc);
    auto client = create_func();
    if (!client)
    {
        LOG_ERROR("Got invalid client from factory func.");
    }

    auto client_plugin = std::make_unique<HsmObjectStoreClientPlugin>(this, client);
    return client_plugin;
}

HsmObjectStorePluginFactory::HsmObjectStorePluginFactory(const std::string& name)
    : ostk::PluginFactory(name)
{
}

std::unique_ptr<ostk::PluginHandle> HsmObjectStorePluginFactory::createHandle(void* rawHandle) const
{
    return std::make_unique<HsmObjectStorePluginHandle>(rawHandle, mName);
}