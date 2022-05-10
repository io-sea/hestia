#include "types.h"
#include <nlohmann/json.hpp>

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
    /// @param oid ID of the object to check for existence
    ///
    virtual int put_meta_data(const struct hsm_obj& obj) = 0;

    ///
    /// @brief  Send the metadata associated with an object to the key-value
    ///         store
    ///
    /// @param oid ID of the object to set attributes for
    /// @param attrs JSON string representing the attributes to set
    ///
    virtual int put_meta_data(
        const struct hsm_uint& oid, const nlohmann::json& attrs) = 0;

    ///
    /// @brief Retrieve the metadata associated with an object from teh key-value store and populate the object with it
    ///
    /// @param hsm_obj Object to retrieve the metadata for
    ///
    virtual int get_meta_data(struct hsm_obj& obj) = 0;

    ///
    /// @brief Remove an object from the KV_store
    ///
    /// @param oid ID of the object to be removed
    ///
    virtual int remove(const struct hsm_uint& oid) = 0;
};

}  // namespace kv
}  // namespace hestia
