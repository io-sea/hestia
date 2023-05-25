#include "BasicDataPlacementEngine.h"

namespace hestia {
BasicDataPlacementEngine::BasicDataPlacementEngine(TierService* tier_service) :
    DataPlacementEngine(tier_service)
{
}

std::uint8_t BasicDataPlacementEngine::choose_tier(
    const std::size_t length, const std::uint8_t hint)
{
    (void)length;
    return hint;
}
}  // namespace hestia
