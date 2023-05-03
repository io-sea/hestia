#pragma once

#include "ObjectStoreClient.h"

#include "PluginHandle.h"
#include "PluginLoader.h"

namespace hestia {
class ObjectStoreClientPlugin : public PluginResource {
  public:
    using Ptr = std::unique_ptr<ObjectStoreClientPlugin>;
    ObjectStoreClientPlugin(
        const PluginHandle* plugin_handle, ObjectStoreClient* client);

    virtual ~ObjectStoreClientPlugin();

    ObjectStoreClient* get_client() const;

  private:
    ObjectStoreClient* m_client{nullptr};
};

class ObjectStorePluginHandle : public PluginHandle {
  public:
    ObjectStorePluginHandle(void* raw_handle, const std::string& name);

    void destroy_resource(PluginResource* resource) const override;

  protected:
    std::unique_ptr<PluginResource> load_resource_with_factory_func()
        const override;
};

class ObjectStorePluginFactory : public PluginFactory {
  public:
    ObjectStorePluginFactory(const std::string& name);

    std::unique_ptr<PluginHandle> create_handle(
        void* raw_handle) const override;
};
}  // namespace hestia