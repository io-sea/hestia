#include "types.h"

namespace hestia {
namespace kv {

///
/// @brief virtual key value store class to be implemented by key value store
///        solutions
///
class Kv_store {
  public:
    ///
    /// @brief default constructor
    ///
    Kv_store(){};
    ///
    /// @brief default destructor
    ///
    virtual ~Kv_store(){};

    ///
    /// @brief check if an object exists
    ///
    virtual bool object_exists(const struct hsm_uint& oid) = 0;

    ///
    /// @brief  Send the metadata associated with an object to the key-value
    ///         store
    ///
    /// @param hsm_obj Object containing relevant metatdata
    ///
    virtual int put_meta_data(const struct hsm_obj& obj) = 0;
};

}  // namespace kv
}  // namespace hestia
