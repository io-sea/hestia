#include "TestContext.h"

#include "TestDirectories.h"

#include "Logger.h"

std::unique_ptr<hestia::ObjectStoreClientPlugin>
TestContext::get_object_store_plugin(const std::string& plugin_name)
{
    hestia::ObjectStorePluginFactory plugin_factory("libhestia_" + plugin_name);
    auto plugin = m_plugin_loader.load_plugin_resource(
        TestDirectories::get_plugin_dir(), plugin_factory);

    auto raw_plugin = plugin.get();
    if (auto typed_plugin =
            dynamic_cast<hestia::ObjectStoreClientPlugin*>(raw_plugin)) {
        auto cast_ptr =
            std::unique_ptr<hestia::ObjectStoreClientPlugin>(typed_plugin);
        plugin.release();
        return cast_ptr;
    }
    else {
        LOG_ERROR("Failed to cast resource to Object Store type");
        return nullptr;
    }
}

TestContext& TestContext::get()
{
    static TestContext instance;
    return instance;
}