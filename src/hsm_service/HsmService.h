#pragma once

#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"
#include "HsmObject.h"

#include <ostk/Stream.h>

class MultiBackendHsmObjectStoreClient;
class KeyValueStore;
class HsmStoreInterface;

class DataPlacementEngine;

class HsmObjectAdapter;
class HsmActionAdapter;

class HsmService
{
public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(std::unique_ptr<KeyValueStore> kvStore, 
        std::unique_ptr<MultiBackendHsmObjectStoreClient> objectStore, 
        std::unique_ptr<DataPlacementEngine> placementEngine);

    ~HsmService();

    [[nodiscard]] HsmServiceResponse::Ptr makeRequest(const HsmServiceRequest& request, ostk::Stream* stream = nullptr) noexcept;

private:
    HsmServiceResponse::Ptr get(const HsmServiceRequest& request, ostk::Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr put(const HsmServiceRequest& request, ostk::Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr copy(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr move(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove_all(const HsmServiceRequest& request) noexcept;
    
    void listAttributes(HsmObject& object);
    void listObjects(uint8_t tier, std::vector<HsmObject>& objects);
    void listTiers(HsmObject& object, std::vector<uint8_t>& tiers);

    std::unique_ptr<HsmStoreInterface> mStore;
    std::unique_ptr<DataPlacementEngine> mDataPlacementEngine;

    std::unique_ptr<HsmObjectAdapter> mObjectAdapter;
    std::unique_ptr<HsmActionAdapter> mActionAdapter;
};