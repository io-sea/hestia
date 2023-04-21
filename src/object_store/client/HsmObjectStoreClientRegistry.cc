#include "HsmObjectStoreClientRegistry.h"

#include "FileHsmObjectStoreClient.h"

#include <ostk/FileObjectStoreClient.h>

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::directory_entry>& search_paths) :
    mSeachPaths(searchPaths)
{
}

bool ObjectStorePluginHandler::has_plugin(const std::string& identifier)
{
    return false;
};

ostk::ObjectStoreClient::Ptr ObjectStorePluginHandler::get_client(
    ObjectStoreClientType clientType) const
{
    return nullptr;
}

HsmObjectStoreClientRegistry::HsmObjectStoreClientRegistry(
    ObjectStorePluginHandler::Ptr plugin_handler) :
    mPluginHandler(std::move(pluginHandler))
{
}

bool HsmObjectStoreClientRegistry::is_client_type_available(
    ObjectStoreClientType client_type) const
{
    if (client_type.mSource == ObjectStoreClientType::Source::BUILT_IN) {
        return true;
    }
    return mPluginHandler->hasPlugin(clientType.mIdentifier);
}

ostk::ObjectStoreClient::Ptr HsmObjectStoreClientRegistry::get_client(
    ObjectStoreClientType clientType) const
{
    if (clientType.mSource == ObjectStoreClientType::Source::BUILT_IN) {
        if (clientType.mIdentifier
            == FileHsmObjectStoreClient::getRegistryIdentifier()) {
            return FileHsmObjectStoreClient::Create();
        }
        else if (
            clientType.mIdentifier
            == ostk::FileObjectStoreClient::getRegistryIdentifier()) {
            return ostk::FileObjectStoreClient::Create();
        }
    }
    else {
        return mPluginHandler->getClient(clientType);
    }
    return nullptr;
}
