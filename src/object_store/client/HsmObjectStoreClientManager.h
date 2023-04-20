#pragma once

#include "HsmObjectStoreClientRegistry.h"

#include <unordered_map>

class HsmObjectStoreClientManager
{
public:
    using Ptr = std::unique_ptr<HsmObjectStoreClientManager>;

    HsmObjectStoreClientManager(HsmObjectStoreClientRegistry::Ptr clientRegistry);

    void setupClients(const TierBackendRegistry& tierBackendRegsitry);

    ostk::ObjectStoreClient* getClient(ObjectStoreClientType clientType) const;

    ostk::ObjectStoreClient* getClient(uint8_t tierId) const;

    HsmObjectStoreClient* getHsmClient(ObjectStoreClientType clientType) const;

    HsmObjectStoreClient* getHsmClient(uint8_t tierId) const;

    bool isHsmClient(uint8_t tierId) const;

    bool haveSameClientTypes(uint8_t tierId0, uint8_t tierId1) const;

private:
    HsmObjectStoreClientRegistry::Ptr mClientRegistry;
    TierBackendRegistry mTierBackendRegistry;
    std::unordered_map<std::string, ostk::ObjectStoreClient::Ptr> mClients;
    std::unordered_map<std::string, HsmObjectStoreClient::Ptr> mHsmClients;
};