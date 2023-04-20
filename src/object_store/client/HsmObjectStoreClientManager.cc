#include "HsmObjectStoreClientManager.h"

HsmObjectStoreClientManager::HsmObjectStoreClientManager(HsmObjectStoreClientRegistry::Ptr clientRegistry)
    : mClientRegistry(std::move(clientRegistry))
{

}

ostk::ObjectStoreClient* HsmObjectStoreClientManager::getClient(ObjectStoreClientType clientType) const
{
    if (const auto iter = mHsmClients.find(clientType.mIdentifier); iter != mHsmClients.end())
    {
        return iter->second.get();
    }
    else
    {
        if (const auto iter = mClients.find(clientType.mIdentifier); iter != mClients.end())
        {
            return iter->second.get();
        }
        else
        {
            return nullptr;
        }
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::getHsmClient(ObjectStoreClientType clientType) const
{
    if (!clientType.isHsm())
    {
        throw std::runtime_error("Requested HSM client for non-hsm client type.");
    }

    if (const auto iter = mHsmClients.find(clientType.mIdentifier); iter != mHsmClients.end())
    {
        return iter->second.get();
    }
    else
    {
        return nullptr;
    }
}

bool HsmObjectStoreClientManager::isHsmClient(uint8_t tierId) const
{
    if (const auto iter = mTierBackendRegistry.find(tierId); iter != mTierBackendRegistry.end())
    {
        return iter->second.isHsm();
    }
    return false;
}

ostk::ObjectStoreClient* HsmObjectStoreClientManager::getClient(uint8_t tierId) const
{
    if (const auto iter = mTierBackendRegistry.find(tierId); iter != mTierBackendRegistry.end())
    {
        return getClient(iter->second);
    }
    else
    {
        return nullptr;
    }
}

HsmObjectStoreClient* HsmObjectStoreClientManager::getHsmClient(uint8_t tierId) const
{
    if (const auto iter = mTierBackendRegistry.find(tierId); iter != mTierBackendRegistry.end())
    {
        return getHsmClient(iter->second);
    }
    else
    {
        return nullptr;
    }
}

bool HsmObjectStoreClientManager::haveSameClientTypes(uint8_t tierId0, uint8_t tierId1) const
{
    auto type0 = mTierBackendRegistry.find(tierId0)->second;
    auto type1 = mTierBackendRegistry.find(tierId1)->second;
    return type0.mIdentifier == type1.mIdentifier;
}

void HsmObjectStoreClientManager::setupClients(const TierBackendRegistry& tierBackendRegsitry)
{
    mTierBackendRegistry = tierBackendRegsitry;

    for(const auto& item : mTierBackendRegistry)
    {
        if(!mClientRegistry->isClientTypeAvailable(item.second))
        {
            std::string msg = "Client " + item.second.mIdentifier;
            msg += " not available. Requested for tier id: " + std::to_string(item.first); 
            throw std::runtime_error(msg);
        }

        if (item.second.isHsm())
        {
            if (!getHsmClient(item.second))
            {
                HsmObjectStoreClient::Ptr hsm_client;
                auto client = mClientRegistry->getClient(item.second);

                if(auto raw_client = dynamic_cast<HsmObjectStoreClient*>(client.get())) 
                {
                    client.release();
                    auto hsm_client = std::unique_ptr<HsmObjectStoreClient>(raw_client);
                    mHsmClients[item.second.mIdentifier] = std::move(hsm_client);
                }
                else
                {
                    throw std::runtime_error("Failed to retreive hsm client - bad cast.");
                }
            }  
        }
        else if (!getClient(item.second))
        {
            mClients[item.second.mIdentifier] = mClientRegistry->getClient(item.second);
        }  
    }
}