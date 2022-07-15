#include "types.h"

namespace hestia {
namespace obj {

///
/// @brief virtual object storage class to be implemented by backend storage
///        solutions
///
class Object_store {
  public:
    ///
    /// @brief default constructor
    ///
    Object_store(){};

    ///
    /// @brief default destructor
    ///
    virtual ~Object_store(){};

    ///
    /// @brief  Send the actual data to object storage
    ///
    /// @param oid ID of the object we are sending the data to
    /// @param buf Address of the data
    /// @param length Amount of data to send in bytes
    /// @param target_tier Tier we are sending the data to
    ///
    virtual int put(
        const struct hsm_uint& oid,
        const void* buf,
        const std::size_t length,
        const std::uint8_t target_tier = 0) = 0;

    ///
    /// @brief  Retrieve the actual data from the object store
    ///
    /// @param oid ID of the object we are retrieving the data from
    /// @param buf Buffer to store the data retrieved
    /// @param length Amount of data to retrieve in bytes
    /// @param src_tier Tier the data resides on
    ///
    virtual int get(
        const struct hsm_uint& oid,
        void* buf,
        const std::size_t length,
        const std::uint8_t src_tier = 0) = 0;

    ///
    /// @brief  Remove the object for the backend object storage
    ///
    /// @param oid ID of the object to be removed
    /// @param tier the object resides on
    ///
    virtual int remove(const struct hsm_uint& oid, const std::uint8_t tier) = 0;
};

}  // namespace obj
}  // namespace hestia
