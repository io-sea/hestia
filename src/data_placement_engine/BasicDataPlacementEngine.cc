#include "BasicDataPlacementEngine.h"

std::uint8_t BasicDataPlacementEngine::choose_tier(
    const std::size_t length, const std::uint8_t hint)
{
    (void)length;
    (void)hint;

    // std::uniform_int_distribution<> distrib(0, m_tiers.size() - 1);

    //    const auto tier  = distrib(m_gen);
    //  auto chosen_tier = std::next(m_tiers.begin(), tier);

    //    return chosen_tier->id;
    return hint;
}

std::list<hestia::HsmTier> BasicDataPlacementEngine::get_tier_info()
{
    return m_tiers;
}

std::vector<uint8_t> BasicDataPlacementEngine::list_tiers()
{
    std::vector<uint8_t> tids;

    for (auto it = m_tiers.begin(); it != m_tiers.end(); ++it) {
        tids.push_back(static_cast<int>(it->id));
    }

    return tids;
}
