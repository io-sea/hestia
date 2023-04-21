#pragma once

#include "HsmTier.h"

#include <list>
#include <vector>

class DataPlacementEngine {
  public:
    DataPlacementEngine(){};

    virtual ~DataPlacementEngine() = default;

    virtual std::uint8_t choose_tier(
        const std::size_t length, const std::uint8_t hint = 0) = 0;

    virtual void reorganise() const = 0;

    virtual std::list<hestia::HsmTier> get_tier_info() = 0;

    virtual std::vector<uint8_t> list_tiers() = 0;

    void set_tiers(const std::list<hestia::HsmTier>& tiers) { m_tiers = tiers; }

  protected:
    std::list<hestia::HsmTier> m_tiers;
};
