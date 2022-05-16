#include "types.h"
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
    /// @returns boolean for whether the object exists
    virtual bool object_exists(const struct hsm_uint& oid) = 0;

    ///
    /// @brief  Send the metadata associated with an object to the key-value
    ///         store
    ///
    /// @param oid ID of the object to check for existence
    ///
    /// @returns 0 on success or error code on failure
    virtual int put_meta_data(const struct hsm_obj& obj) = 0;

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
    /// @brief Retrieve the metadata associated with an object from the key-value store and populate the object with it
    ///
    /// @param hsm_obj Object to retrieve the metadata for
    ///
    /// @returns 0 on success or error code on failure
    virtual int get_meta_data(struct hsm_obj& obj) = 0;

    ///
    /// @brief Retrieve all entries of the metadata associated with an object
    ///
    /// @param hsm_uint ID of the object to retrieve attributed for
    ///
    /// @param nlohmann::json JSON string to hold the retrieved attributes
    virtual int get_meta_data(
        const struct hsm_uint& oid, nlohmann::json& attrs) = 0;

    ///
    /// @brief Retrieve entries of the metadata associated with an object, corresponding to the given keys
    ///
    /// @param hsm_uint ID of the object to retrieve attributed for
    ///
    /// @param char* attr_keys List of the keys we want to retrieve values for
    ///
    /// @param nlohmann::json JSON string to hold the retrieved attributes
    virtual int get_meta_data(
        const struct hsm_uint& oid, char* attr_keys, nlohmann::json& attrs) = 0;

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
    /// @param oids Array to store the list of oids in
    ///
    /// @returns array of IDs for the objects found
    virtual std::vector<struct hsm_uint> list(const std::uint8_t tier = 0) = 0;
};

}  // namespace kv
}  // namespace hestia
