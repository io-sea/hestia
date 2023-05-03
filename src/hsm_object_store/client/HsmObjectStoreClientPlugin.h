#pragma once

#include "HsmObjectStoreClient.h"

#include "PluginHandle.h"
#include "PluginLoader.h"

namespace hestia {
class HsmObjectStoreClientPlugin : public PluginResource {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientPlugin>;
    HsmObjectStoreClientPlugin(
        const PluginHandle* plugin_handle, HsmObjectStoreClient* client);

    virtual ~HsmObjectStoreClientPlugin();

    HsmObjectStoreClient* get_client() const;

  private:
    HsmObjectStoreClient* m_client{nullptr};
};

class HsmObjectStorePluginHandle : public PluginHandle {
  public:
    HsmObjectStorePluginHandle(void* raw_handle, const std::string& name);

    void destroy_resource(PluginResource* resource) const override;

  protected:
    std::unique_ptr<PluginResource> load_resource_with_factory_func()
        const override;
};

class HsmObjectStorePluginFactory : public PluginFactory {
  public:
    HsmObjectStorePluginFactory(const std::string& name);

    std::unique_ptr<PluginHandle> create_handle(
        void* raw_handle) const override;
};
}  // namespace hestia
