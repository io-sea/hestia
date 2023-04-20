#pragma once

#include "CopyTool.h"
#include "DataPlacementEngineRegistry.h"
#include "HsmObjectStoreClientRegistry.h"
#include "KeyValueStoreClientRegistry.h"

class MultiBackendHsmObjectStoreClient;
class KeyValueStore;
class DataPlacementEngine;

namespace hestia {
struct HestiaConfig {
    TierBackendRegistry mTierBackendRegistry;
    KeyValueStoreType mKeyValueStoreType;
    PlacementEngineType mPlacementEngineType;
    CopyToolConfig mCopyToolConfig;
};

class HestiaConfigurator {
  public:
    int initialize(const HestiaConfig& config);

  private:
    std::unique_ptr<MultiBackendHsmObjectStoreClient> setUpObjectStore();

    std::unique_ptr<KeyValueStore> setUpKeyValueStore();

    std::unique_ptr<DataPlacementEngine> setUpDataPlacementEngine();

    HestiaConfig mConfig;
};
}  // namespace hestia