#include "DataPlacementEngineFactory.h"

namespace hestia {
std::string DataPlacementEngineFactory::to_string(
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

bool DataPlacementEngineFactory::is_placement_engine_available(
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


std::unique_ptr<DataPlacementEngine> DataPlacementEngineFactory::get_engine(
    PlacementEngineType placement_engine_type, TierService* tier_service)
{
    switch (placement_engine_type) {
        case PlacementEngineType::BASIC:
            return std::make_unique<BasicDataPlacementEngine>(tier_service);
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