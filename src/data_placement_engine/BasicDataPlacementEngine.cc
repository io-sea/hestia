#include "BasicDataPlacementEngine.h"

std::uint8_t BasicDataPlacementEngine::chooseTier(
    const std::size_t length, const std::uint8_t hint)
{
    // std::uniform_int_distribution<> distrib(0, m_tiers.size() - 1);

    //    const auto tier  = distrib(m_gen);
    //  auto chosen_tier = std::next(m_tiers.begin(), tier);

    //    return chosen_tier->id;
    return hint;
}

std::list<hestia::HsmTier> BasicDataPlacementEngine::getTierInfo()
{
    return mTiers;
}

std::vector<uint8_t> BasicDataPlacementEngine::listTiers()
{
    std::vector<uint8_t> tids;

    for (auto it = mTiers.begin(); it != mTiers.end(); ++it) {
        tids.push_back(static_cast<int>(it->id));
    }

    return tids;
}
