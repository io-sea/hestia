#pragma once

#include "BasicDataPlacementEngine.h"

#ifdef HAS_ROBINHOOD
#include "RobinhoodPlacementEngine.h"
#endif

#include <memory>
#include <string>

namespace hestia {
enum class PlacementEngineType { BASIC, ROBINHOOD };

class DataPlacementEngineRegistry {

  public:
    static std::string to_string(PlacementEngineType placement_engine_type);

    static bool is_placement_engine_available(
        PlacementEngineType placement_engine_type);

    static std::unique_ptr<DataPlacementEngine> get_engine(
        PlacementEngineType placement_engine_type, TierService* tier_service);
};

}  // namespace hestia