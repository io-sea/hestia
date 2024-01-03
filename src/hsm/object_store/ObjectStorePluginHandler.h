#pragma once

#include "ObjectStoreBackend.h"

#include "HsmObjectStoreClientPlugin.h"
#include "ObjectStoreClientPlugin.h"
#include "PluginLoader.h"

#include <filesystem>
#include <vector>

namespace hestia {

class ObjectStorePluginHandler {
  public:
    using Ptr = std::unique_ptr<ObjectStorePluginHandler>;

    ObjectStorePluginHandler(
        const std::vector<std::filesystem::path>& search_paths);

    bool has_plugin(const ObjectStoreBackend& client_spec);

    ObjectStoreClientPlugin::Ptr get_object_store_plugin(
        const ObjectStoreBackend& client_spec);

    HsmObjectStoreClientPlugin::Ptr get_hsm_object_store_plugin(
        const ObjectStoreBackend& client_spec);

  private:
    PluginLoader m_plugin_loader;
    std::vector<std::filesystem::path> m_search_paths;
};

}  // namespace hestia