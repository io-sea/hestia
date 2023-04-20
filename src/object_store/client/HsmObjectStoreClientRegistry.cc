#include "HsmObjectStoreClientRegistry.h"

#include "FileHsmObjectStoreClient.h"

#include <ostk/FileObjectStoreClient.h>

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::directory_entry>& searchPaths) :
    mSeachPaths(searchPaths)
{
}

bool ObjectStorePluginHandler::hasPlugin(const std::string& identifier)
{
    return false;
};

ostk::ObjectStoreClient::Ptr ObjectStorePluginHandler::getClient(
    ObjectStoreClientType clientType) const
{
    return nullptr;
}

HsmObjectStoreClientRegistry::HsmObjectStoreClientRegistry(
    ObjectStorePluginHandler::Ptr pluginHandler) :
    mPluginHandler(std::move(pluginHandler))
{
}

bool HsmObjectStoreClientRegistry::isClientTypeAvailable(
    ObjectStoreClientType clientType) const
{
    if (clientType.mSource == ObjectStoreClientType::Source::BUILT_IN) {
        return true;
    }
    return mPluginHandler->hasPlugin(clientType.mIdentifier);
}

ostk::ObjectStoreClient::Ptr HsmObjectStoreClientRegistry::getClient(
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
