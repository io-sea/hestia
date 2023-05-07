#include "HsmObjectStoreClientRegistry.h"

#include "FileHsmObjectStoreClient.h"

#include "FileObjectStoreClient.h"

#include "Logger.h"

namespace hestia {
ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::directory_entry>& search_paths) :
    m_search_paths(search_paths)
{
}

bool ObjectStorePluginHandler::has_plugin(const std::string& identifier)
{
    (void)identifier;
    return false;
}

ObjectStoreClient::Ptr ObjectStorePluginHandler::get_client(
    ObjectStoreClientType client_type) const
{
    (void)client_type;
    return nullptr;
}

HsmObjectStoreClientRegistry::HsmObjectStoreClientRegistry(
    ObjectStorePluginHandler::Ptr plugin_handler) :
    m_plugin_handler(std::move(plugin_handler))
{
}

bool HsmObjectStoreClientRegistry::is_client_type_available(
    ObjectStoreClientType client_type) const
{
    if (client_type.m_source == ObjectStoreClientType::Source::BUILT_IN) {
        return true;
    }
    return m_plugin_handler->has_plugin(client_type.m_identifier);
}

ObjectStoreClient::Ptr HsmObjectStoreClientRegistry::get_client(
    ObjectStoreClientType client_type) const
{
    if (client_type.m_source == ObjectStoreClientType::Source::BUILT_IN) {
        if (client_type.m_identifier
            == FileHsmObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileHsmObjectStoreClient");
            return FileHsmObjectStoreClient::create();
        }
        else if (
            client_type.m_identifier
            == hestia::FileObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileObjectStoreClient");
            return hestia::FileObjectStoreClient::create();
        }
    }
    else {
        return m_plugin_handler->get_client(client_type);
    }
    return nullptr;
}
}  // namespace hestia
