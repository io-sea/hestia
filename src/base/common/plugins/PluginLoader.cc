#include "PluginLoader.h"

#include "Logger.h"
#include "SystemUtils.h"

namespace hestia {

PluginFactory::PluginFactory(const std::string& name) : m_name(name) {}

const std::string& PluginFactory::get_name() const
{
    return m_name;
}

std::string PluginLoader::get_plugin_extension() const
{
#ifdef __linux__
    return ".so";
#else
    return ".dylib";
#endif
}

std::unique_ptr<PluginResource> PluginLoader::load_plugin_resource(
    const std::vector<std::filesystem::path>& plugin_dirs,
    const PluginFactory& plugin_factory)
{
    auto plugin_iter = m_plugins.find(plugin_factory.get_name());

    if (plugin_iter == m_plugins.end()) {

        OpStatus status;
        void* handle{nullptr};
        for (const auto& dir : plugin_dirs) {
            const auto path =
                dir / (plugin_factory.get_name() + get_plugin_extension());
            auto [op_status, op_handle] =
                SystemUtils::load_module(path.string());
            if (op_status.ok()) {
                status = op_status;
                handle = op_handle;
                break;
            }
        }

        if (handle == nullptr) {
            const auto path =
                plugin_factory.get_name() + get_plugin_extension();
            auto [op_status, op_handle] = SystemUtils::load_module(path);
            status                      = op_status;
            handle                      = op_handle;
        }

        if (!status.ok()) {
            LOG_ERROR(status.message());
            return nullptr;
        }

        auto plugin_handle     = plugin_factory.create_handle(handle);
        auto plugin_handle_ptr = plugin_handle.get();
        m_plugins[plugin_factory.get_name()] = std::move(plugin_handle);
        return plugin_handle_ptr->load_resource();
    }
    else {
        return plugin_iter->second->load_resource();
    }
}
}  // namespace hestia