#include "PluginHandle.h"

#include "Logger.h"
#include "SystemUtils.h"

namespace hestia {

PluginHandle::PluginHandle(void* handle, const std::string& name) :
    m_handle(handle), m_name(name)
{
}

PluginHandle::~PluginHandle()
{
    LOG_INFO("Destroying plugin handle: " << m_name);
    if (const auto status = SystemUtils::close_module(m_handle, m_name);
        !status.ok()) {
        LOG_ERROR(status.message());
    }
}

std::unique_ptr<PluginResource> PluginHandle::load_resource()
{
    if (m_create_func == nullptr) {
        auto [status, symbol] =
            SystemUtils::load_symbol(m_handle, m_name, "create");
        if (!status.ok()) {
            LOG_ERROR(status.message());
            return nullptr;
        }
        m_create_func = symbol;
    }

    if (m_destroy_func == nullptr) {
        auto [status, symbol] =
            SystemUtils::load_symbol(m_handle, m_name, "destroy");
        if (!status.ok()) {
            LOG_ERROR(status.message());
            return nullptr;
        }
        m_destroy_func = symbol;
    }
    return load_resource_with_factory_func();
}

PluginResource::PluginResource(const PluginHandle* plugin_handle) :
    m_plugin_handle(plugin_handle)
{
}

}  // namespace hestia