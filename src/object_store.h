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
    /// @param buf Address of the data
    /// @param length Amount of data to send in bytes
    ///
    virtual int put(
        const struct hsm_uint& oid,
        const void* buf,
        const std::size_t length) = 0;
};

}  // namespace obj
}  // namespace hestia
