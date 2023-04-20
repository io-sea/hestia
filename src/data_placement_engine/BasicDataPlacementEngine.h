#pragma once

#include "DataPlacementEngine.h"
#include <random>


class BasicDataPlacementEngine : public DataPlacementEngine {
  public:
    BasicDataPlacementEngine() : DataPlacementEngine()
    {
        //m_gen.seed(m_rd());
    }

    ~BasicDataPlacementEngine() {}

    std::uint8_t chooseTier(const std::size_t length, const std::uint8_t hint = 0);

    void reorganise() const {}

    std::list<hestia::HsmTier> getTierInfo();

    std::vector<uint8_t> listTiers();

};
