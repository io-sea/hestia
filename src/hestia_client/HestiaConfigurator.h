#pragma once

#include "ErrorUtils.h"
#include "HestiaConfig.h"

class DataPlacementEngine;

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class KeyValueStoreClient;
class TierService;

class HestiaConfigurator {
  public:
    OpStatus initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<MultiBackendHsmObjectStoreClient> set_up_object_store();

    std::unique_ptr<KeyValueStoreClient> set_up_key_value_store();

    std::unique_ptr<DataPlacementEngine> set_up_data_placement_engine(
        TierService* tier_service);

    std::unique_ptr<EventFeed> set_up_event_feed();

    HestiaConfig m_config;
};
}  // namespace hestia