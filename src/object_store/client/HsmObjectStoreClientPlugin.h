#pragma once

#include "HsmObjectStoreClient.h"

#include <ostk/PluginHandle.h>
#include <ostk/PluginLoader.h>

class HsmObjectStoreClientPlugin : public ostk::PluginResource
{
public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientPlugin>;
    HsmObjectStoreClientPlugin(const ostk::PluginHandle* pluginHandle, HsmObjectStoreClient* client);

    virtual ~HsmObjectStoreClientPlugin();

    HsmObjectStoreClient* getClient() const;

private:
    HsmObjectStoreClient* mClient{nullptr};
};

class HsmObjectStorePluginHandle : public ostk::PluginHandle
{
public:
    HsmObjectStorePluginHandle(void* rawHandle, const std::string& name);

    void destroyResource(ostk::PluginResource* resource) const override;

protected:
    std::unique_ptr<ostk::PluginResource> loadResourceWithFactoryFunc() const override;
};

class HsmObjectStorePluginFactory : public ostk::PluginFactory
{
public:
    HsmObjectStorePluginFactory(const std::string& name);

    std::unique_ptr<ostk::PluginHandle> createHandle(void* rawHandle) const override;
};

