#pragma once

#include "PluginLoader.h"

#include "HsmObjectStoreClientPlugin.h"

class TestContext {
  public:
    static TestContext& get();


  private:
    hestia::PluginLoader m_plugin_loader;
};