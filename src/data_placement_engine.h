#include "types.h"
#include <list>

namespace hestia {
namespace dpe {

///
/// @brief virtual key value store class to be implemented by key value store
///        solutions
///
class DPE {
  public:
    ///
    /// @brief default constructor
    ///
    /// @param tiers Set of the available tiers
    ///
    DPE(const std::list<struct hsm_tier>& tiers) : m_tiers(tiers){};

    ///
    /// @brief default destructor
    ///
    virtual ~DPE(){};

    ///
    /// @brief Choose tier
    ///
    /// Choose one of the available tiers as the destination for an object
    ///
    /// @param length Size of the object to place
    /// @param hint Hint at the tier to place the object in
    ///
    /// @returns ID of the tier to put an object to
    virtual std::uint8_t choose_tier(
        const std::size_t length, const std::uint8_t hint = 0) = 0;

    ///
    /// @brief Move objects between tiers
    ///
    virtual void reorganise() const = 0;

    ///
    /// @brief Return array of tier ids
    ///
    virtual std::list<struct hsm_tier> get_tiers_info() = 0;

    ///
    /// @brief Return list of information about the tiers
    ///

    virtual std::vector<uint8_t> list_tiers() = 0;

  protected:
    std::list<struct hsm_tier> m_tiers;
};

}  // namespace dpe
}  // namespace hestia
