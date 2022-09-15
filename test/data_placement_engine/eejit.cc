#include "eejit.h"

std::uint8_t hestia::dpe::Eejit::choose_tier(
    const std::size_t length, const std::uint8_t hint)
{
    // std::uniform_int_distribution<> distrib(0, m_tiers.size() - 1);

    //    const auto tier  = distrib(m_gen);
    //  auto chosen_tier = std::next(m_tiers.begin(), tier);

    //    return chosen_tier->id;
    return hint;
}

std::list<struct hestia::hsm_tier> hestia::dpe::Eejit::get_tiers_info(){
    return m_tiers;
}

