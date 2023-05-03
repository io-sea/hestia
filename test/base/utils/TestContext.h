#pragma once

#include "PluginLoader.h"

#include "ObjectStoreClientPlugin.h"

class TestContext {
  public:
    static TestContext& get();

    std::unique_ptr<hestia::ObjectStoreClientPlugin> get_object_store_plugin(
        const std::string& plugin_name);

  private:
    hestia::PluginLoader m_plugin_loader;
};