#pragma once

#include "HsmObject.h"
#include "HsmServiceRequest.h"
#include "HsmServiceResponse.h"

#include "Stream.h"

class DataPlacementEngine;

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class KeyValueStore;
class HsmStoreInterface;

class HsmObjectAdapter;
class HsmActionAdapter;

class HsmService {
  public:
    using Ptr = std::unique_ptr<HsmService>;

    HsmService(
        std::unique_ptr<KeyValueStore> kv_store,
        std::unique_ptr<MultiBackendHsmObjectStoreClient> object_store,
        std::unique_ptr<DataPlacementEngine> placement_engine);

    ~HsmService();

    [[nodiscard]] HsmServiceResponse::Ptr make_request(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;

  private:
    HsmServiceResponse::Ptr get(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr put(
        const HsmServiceRequest& request, Stream* stream = nullptr) noexcept;
    HsmServiceResponse::Ptr copy(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr move(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove(const HsmServiceRequest& request) noexcept;
    HsmServiceResponse::Ptr remove_all(
        const HsmServiceRequest& request) noexcept;

    void list_attributes(HsmObject& object);
    void list_objects(uint8_t tier, std::vector<HsmObject>& objects);
    void list_tiers(HsmObject& object, std::vector<uint8_t>& tiers);

    std::unique_ptr<HsmStoreInterface> m_store;
    std::unique_ptr<DataPlacementEngine> m_data_placement_engine;

    std::unique_ptr<HsmObjectAdapter> m_object_adapter;
    std::unique_ptr<HsmActionAdapter> m_action_adapter;
};
}  // namespace hestia