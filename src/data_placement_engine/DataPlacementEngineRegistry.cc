#include "DataPlacementEngineRegistry.h"

namespace hestia {
std::string DataPlacementEngineRegistry::to_string(
    PlacementEngineType placement_engine_type)
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

bool DataPlacementEngineRegistry::is_placement_engine_available(
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


std::unique_ptr<DataPlacementEngine> DataPlacementEngineRegistry::get_engine(
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
}  // namespace hestia