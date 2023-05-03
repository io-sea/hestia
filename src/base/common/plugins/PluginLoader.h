#pragma once

#include "PluginHandle.h"

#include <filesystem>
#include <unordered_map>

namespace hestia {

/**
 * @brief This class will create a specialized (typed) PluginHandle given a system plugin handle
 *
 * This class is basically boileplate to allow us to create a specialized
 * PluginHandle by injection into the PluginLoader and specialization of the
 * 'create_handle' virtual method.
 */
class PluginFactory {
  public:
    /**
     * Constructor
     *
     * @param name The name of the plugin to load
     */
    PluginFactory(const std::string& name);

    virtual ~PluginFactory() = default;

    /**
     * Derived classes implement this to create a specialized 'PluginHandle
     * given a 'raw' system handle
     * @param raw_handle a system handle for the plugin
     */
    virtual std::unique_ptr<PluginHandle> create_handle(
        void* raw_handle) const = 0;

    /**
     * The name of the plugin
     * @return The name of the plugin
     */
    const std::string& get_name() const;

  protected:
    std::string m_name;
};

/**
 * @brief This class handles loading and lifecycle of plugins (shared libraries)
 *
 * This class will load and cache named plugins.
 */
class PluginLoader {
  public:
    /**
     * Load the 'default resource' from the plugin specified by the
     * 'PluginFactory'. If the plugin hasn't been previously loaded it is
     * searched for in the 'plugin_dir'
     * @param plugin_dir directory to search for the plugin
     * @param plugin_factory provides details on the plugin to load, particularly the name
     * @return a wrapper around the default resource (C++ object) loaded from the plugin. Nullptr if not found.
     */
    std::unique_ptr<PluginResource> load_plugin_resource(
        const std::filesystem::path& plugin_dir,
        const PluginFactory& plugin_factory);

  private:
    std::string get_plugin_extension() const;

    std::unordered_map<std::string, std::unique_ptr<PluginHandle>> m_plugins;
};

}  // namespace hestia