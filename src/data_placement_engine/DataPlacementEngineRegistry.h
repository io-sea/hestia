#pragma once

#include "BasicDataPlacementEngine.h"

#ifdef HAS_ROBINHOOD
#include "RobinhoodPlacementEngine.h"
#endif

#include <string>

enum class PlacementEngineType { BASIC, ROBINHOOD };

namespace placement_engine_registry {

static std::string to_string(PlacementEngineType placement_engine_type)
{
    switch (placement_engine_type) {
        case PlacementEngineType::BASIC:
            return "BASIC";
        case PlacementEngineType::ROBINHOOD:
            return "ROBINHOOD";
        default:
            return "UNKNOWN";
    }
};

static bool is_placement_engine_available(
    PlacementEngineType placement_engine_type)
{
    switch (placement_engine_type) {
        case PlacementEngineType::BASIC:
            return true;
        case PlacementEngineType::ROBINHOOD:
#ifdef HAS_ROBINHOOD
            return true;
#else
            return false;
#endif
        default:
            return false;
    }
};

static std::unique_ptr<DataPlacementEngine> get_engine(
    PlacementEngineType placement_engine_type)
{
    switch (placement_engine_type) {
        case PlacementEngineType::BASIC:
            return std::make_unique<BasicDataPlacementEngine>();
        case PlacementEngineType::ROBINHOOD:
#ifdef HAS_ROBINHOOD
            return std::make_unique<RobinhoodDataPlacementEngine>();
#else
            return nullptr;
#endif
        default:
            return nullptr;
    }
};

}  // namespace placement_engine_registry