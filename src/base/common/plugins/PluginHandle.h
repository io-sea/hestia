#pragma once

#include <functional>
#include <memory>
#include <string>

namespace hestia {
class PluginResource;

/**
 * @brief Handle to a shared library (.so, .dll, .dylib) with convenience methods
 *
 * This class holds a handle to shared library (plugin). It has convenice
 * methods to load and destroy resources (C++ objects) from the shared library
 * and manage their lifecycle. It wraps each loaded C++ object as a
 * PluginResource.
 *
 * The built-in methods assume only one C++ type is returned from the plugin and
 * is provided by a free-function named 'create'. This object is destroyed by
 * calling a function named 'destroy' pulled in from the shared library.
 */
class PluginHandle {
  public:
    /**
     * Constructor
     *
     * @param handle A system handle to the shared library (plugin)
     * @param name A descriptive name for the plugin - for debugging
     */
    PluginHandle(void* handle, const std::string& name);

    virtual ~PluginHandle();

    /**
     * Supplied by derived classes - should handle destroying the provided
     * wrapped resource via e.g. a class to a function in the shared library.
     * @param resource a wrapper over the resource to be destroyed
     */
    virtual void destroy_resource(PluginResource* resource) const = 0;

    /**
     * This method loads the default resource (c++ object) from the plugin using
     * an assumed factory function naming 'create'. It will also try to load a
     * 'destroy' function for the object with name 'destroy'.
     * @return a wrapper over the loaded object - the called should manage the lifetime via the uptr
     */
    std::unique_ptr<PluginResource> load_resource();

  protected:
    virtual std::unique_ptr<PluginResource> load_resource_with_factory_func()
        const = 0;

    std::string m_name;
    void* m_handle{nullptr};

    void* m_create_func{nullptr};
    void* m_destroy_func{nullptr};
};

/**
 * @brief Wrapper over a C++ object loaded from a plugin to manage its lifecycle.
 *
 * This class wraps a resource returned from the shared library - it is intended
 * to manage the lifecycle when combined with the PluginResource class. When
 * PluginResource goes out of scope its owned object can be destroyed in the
 * shared library - although this is actually only implemented in derived
 * classes specializing the resource type.
 */

class PluginResource {
  public:
    /**
     * Constructor
     *
     * @param plugin_handle The plugin we have loaded this resource from - used in derived class dtors to destory wrapped object
     */
    PluginResource(const PluginHandle* plugin_handle);

    virtual ~PluginResource() = default;

  protected:
    const PluginHandle* m_plugin_handle{nullptr};
};

}  // namespace hestia