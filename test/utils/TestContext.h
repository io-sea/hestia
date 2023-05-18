#pragma once

#include "PluginLoader.h"

#include "HsmObjectStoreClientPlugin.h"
#include "ObjectStoreClientPlugin.h"

class TestContext {
  public:
    static TestContext& get();

    std::unique_ptr<hestia::ObjectStoreClientPlugin> get_object_store_plugin(
        const std::string& plugin_name);

    std::unique_ptr<hestia::HsmObjectStoreClientPlugin>
    get_hsm_object_store_plugin(const std::string& plugin_name);

  private:
    hestia::PluginLoader m_plugin_loader;
};