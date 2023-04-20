#pragma once

#include "HsmTier.h"

#include <list>
#include <vector>

class DataPlacementEngine {
  public:
    DataPlacementEngine(){};

    virtual ~DataPlacementEngine() = default;

    virtual std::uint8_t chooseTier(
        const std::size_t length, const std::uint8_t hint = 0) = 0;

    virtual void reorganise() const = 0;

    virtual std::list<hestia::HsmTier> getTierInfo() = 0;

    virtual std::vector<uint8_t> listTiers() = 0;

    void setTiers(const std::list<hestia::HsmTier>& tiers) { mTiers = tiers; }

  protected:
    std::list<hestia::HsmTier> mTiers;
};
