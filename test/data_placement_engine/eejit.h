#include "../../src/data_placement_engine.h"
#include <random>

namespace hestia {
namespace dpe {

/// @copydoc DPE
class Eejit : public DPE {
  public:
    /// @copydoc DPE::DPE
    Eejit(const std::list<struct hsm_tier>& tiers) : DPE(tiers)
    {
        m_gen.seed(m_rd());
    }

    /// @copydoc DPE::~DPE
    ~Eejit() {}

    /// @copydoc DPE::choose_tier
    std::uint8_t choose_tier(
        const std::size_t length, const std::uint8_t hint = 0);

    /// @copydoc DPE::reorganise
    void reorganise() const {}

    /// @copydoc DPE::get_tiers_info
    std::list<struct hsm_tier> get_tiers_info();

  private:
    std::random_device m_rd;
    std::mt19937 m_gen;
};

}  // namespace dpe
}  // namespace hestia
