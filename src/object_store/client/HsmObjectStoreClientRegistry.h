#pragma once

#include "HsmObjectStoreClient.h"

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>

struct ObjectStoreClientType {
    enum class Type { HSM, BASIC };

    enum class Source { BUILT_IN, PLUGIN, MOCK };

    ObjectStoreClientType(
        Type clientType, Source source, const std::string& identifier) :
        mType(clientType), mSource(source), mIdentifier(identifier)
    {
    }

    bool isHsm() const { return mType == Type::HSM; }

    Type mType{Type::BASIC};
    Source mSource{Source::BUILT_IN};
    std::string mIdentifier;
};

class ObjectStorePluginHandler {
  public:
    using Ptr = std::unique_ptr<ObjectStorePluginHandler>;

    ObjectStorePluginHandler(
        const std::vector<std::filesystem::directory_entry>& searchPaths);

    bool hasPlugin(const std::string& identifier);

    ostk::ObjectStoreClient::Ptr getClient(
        ObjectStoreClientType clientType) const;

  private:
    std::vector<std::filesystem::directory_entry> mSeachPaths;
};

class HsmObjectStoreClientRegistry {
  public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientRegistry>;
    HsmObjectStoreClientRegistry(ObjectStorePluginHandler::Ptr pluginHandler);

    bool isClientTypeAvailable(ObjectStoreClientType clientType) const;

    ostk::ObjectStoreClient::Ptr getClient(
        ObjectStoreClientType clientType) const;

  private:
    ObjectStorePluginHandler::Ptr mPluginHandler;
};

using TierBackendRegistry = std::unordered_map<uint8_t, ObjectStoreClientType>;