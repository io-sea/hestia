#pragma once

#include "HestiaConfig.h"

class DataPlacementEngine;

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class KeyValueStore;

class HestiaConfigurator {
  public:
    int initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<MultiBackendHsmObjectStoreClient> set_up_object_store();

    std::unique_ptr<KeyValueStore> set_up_key_value_store();

    std::unique_ptr<DataPlacementEngine> set_up_data_placement_engine();

    HestiaConfig m_config;
};
}  // namespace hestia