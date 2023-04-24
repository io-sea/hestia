#include "../types.h"
#include <vector>

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
    /// @param oid ID of the object to check for existence
    ///
    /// @returns boolean for whether the object exists
    virtual bool object_exists(const struct hsm_uint& oid) = 0;

    ///
    /// @brief  Send the metadata associated with an object to the key-value
    ///         store
    ///
    /// @param oid ID of the object to set attributes for
    /// @param attrs JSON string representing the attributes to set
    ///
    /// @returns 0 on success or error code on failure
    virtual int put_meta_data(
        const struct hsm_uint& oid, const nlohmann::json& attrs) = 0;


    ///
    /// @brief Retrieve all entries of the metadata associated with an object
    ///
    /// @param oid ID of the object to retrieve attributed for
    ///
    /// @returns JSON string to hold the retrieved attributes
    virtual nlohmann::json get_meta_data(const struct hsm_uint& oid) = 0;

    ///
    /// @brief Retrieve entries of the metadata associated with an object, corresponding to the given keys
    ///
    /// @param oid ID of the object to retrieve attributed for
    ///
    /// @param attr_keys List of the keys we want to retrieve values for
    ///
    /// @returns JSON string to hold the retrieved attributes
    virtual nlohmann::json get_meta_data(
        const struct hsm_uint& oid, const std::string& attr_keys) = 0;

    ///
    /// @brief Remove an object from the KV_store
    ///
    /// @param oid ID of the object to be removed
    ///
    /// @returns 0 on success or error code on failure
    virtual int remove(const struct hsm_uint& oid) = 0;

    ///
    /// @brief List
    ///
    /// @param tier Storage tier from which to list objects
    ///
    /// @returns array of IDs for the objects found
    virtual std::vector<struct hsm_uint> list(const std::uint8_t tier = 0) = 0;
};

}  // namespace kv
}  // namespace hestia
