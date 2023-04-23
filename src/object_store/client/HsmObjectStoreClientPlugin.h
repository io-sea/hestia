#pragma once

#include "HsmObjectStoreClient.h"

#include <ostk/PluginHandle.h>
#include <ostk/PluginLoader.h>

class HsmObjectStoreClientPlugin : public ostk::PluginResource {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientPlugin>;
    HsmObjectStoreClientPlugin(
        const ostk::PluginHandle* plugin_handle, HsmObjectStoreClient* client);

    virtual ~HsmObjectStoreClientPlugin();

    HsmObjectStoreClient* get_client() const;

  private:
    HsmObjectStoreClient* m_client{nullptr};
};

class HsmObjectStorePluginHandle : public ostk::PluginHandle {
  public:
    HsmObjectStorePluginHandle(void* raw_handle, const std::string& name);

    void destroyResource(ostk::PluginResource* resource) const override;

  protected:
    std::unique_ptr<ostk::PluginResource> loadResourceWithFactoryFunc()
        const override;
};

class HsmObjectStorePluginFactory : public ostk::PluginFactory {
  public:
    HsmObjectStorePluginFactory(const std::string& name);

    std::unique_ptr<ostk::PluginHandle> createHandle(
        void* raw_handle) const override;
};
