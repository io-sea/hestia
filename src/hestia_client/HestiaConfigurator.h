#pragma once

#include "ErrorUtils.h"
#include "HestiaConfig.h"

class DataPlacementEngine;

namespace hestia {
class MultiBackendHsmObjectStoreClient;
class HsmKeyValueStore;

class HestiaConfigurator {
  public:
    OpStatus initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<MultiBackendHsmObjectStoreClient> set_up_object_store();

    std::unique_ptr<HsmKeyValueStore> set_up_key_value_store();

    std::unique_ptr<DataPlacementEngine> set_up_data_placement_engine();

    HestiaConfig m_config;
};
}  // namespace hestia