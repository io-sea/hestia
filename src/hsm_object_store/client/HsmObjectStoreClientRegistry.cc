#include "HsmObjectStoreClientRegistry.h"

#include "FileHsmObjectStoreClient.h"

#include <ostk/FileObjectStoreClient.h>

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::directory_entry>& search_paths) :
    m_search_paths(search_paths)
{
}

bool ObjectStorePluginHandler::has_plugin(const std::string& identifier)
{
    return false;
};

ostk::ObjectStoreClient::Ptr ObjectStorePluginHandler::get_client(
    ObjectStoreClientType client_type) const
{
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

ostk::ObjectStoreClient::Ptr HsmObjectStoreClientRegistry::get_client(
    ObjectStoreClientType client_type) const
{
    if (client_type.m_source == ObjectStoreClientType::Source::BUILT_IN) {
        if (client_type.m_identifier
            == FileHsmObjectStoreClient::get_registry_identifier()) {
            return FileHsmObjectStoreClient::create();
        }
        else if (
            client_type.m_identifier
            == ostk::FileObjectStoreClient::getRegistryIdentifier()) {
            return ostk::FileObjectStoreClient::Create();
        }
    }
    else {
        return m_plugin_handler->get_client(client_type);
    }
    return nullptr;
}
