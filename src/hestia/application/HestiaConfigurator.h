#pragma once

#include "ErrorUtils.h"
#include "HestiaConfig.h"

class DataPlacementEngine;

namespace hestia {
class DistributedHsmObjectStoreClient;
class KeyValueStoreClient;

class TierService;
class ObjectService;

class HestiaConfigurator {
  public:
    OpStatus initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<KeyValueStoreClient> set_up_key_value_store();

    std::unique_ptr<DataPlacementEngine> set_up_data_placement_engine(
        TierService* tier_service);

    std::unique_ptr<EventFeed> set_up_event_feed();

    bool set_up_tiers(TierService* tier_service);

    HestiaConfig m_config;
};
}  // namespace hestia