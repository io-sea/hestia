#pragma once

#include "TierService.h"
#include <vector>

namespace hestia {
class DataPlacementEngine {
  public:
    DataPlacementEngine(TierService* tier_service) :
        m_tier_service(tier_service){};

    virtual ~DataPlacementEngine() = default;

    virtual std::uint8_t choose_tier(
        const std::size_t length, const std::uint8_t hint = 0) = 0;

  protected:
    TierService* m_tier_service{nullptr};
};
}  // namespace hestia
