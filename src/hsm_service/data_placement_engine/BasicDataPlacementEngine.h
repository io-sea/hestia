#pragma once

#include "DataPlacementEngine.h"

namespace hestia {

class BasicDataPlacementEngine : public DataPlacementEngine {
  public:
    BasicDataPlacementEngine(TierService* tier_service);

    virtual ~BasicDataPlacementEngine() = default;

    std::uint8_t choose_tier(
        const std::size_t length, const std::uint8_t hint = 0) override;
};
}  // namespace hestia
