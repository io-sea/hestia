#include "types.h"
#include <list>
#include <vector>

namespace hestia {

/// @brief Creates an object in the object store and an entry for it in the key value store
///
/// @param oid ID of the object to be created
/// @param obj Object to be created
///

void create_object(const struct hsm_uint& oid, struct hsm_obj& obj);

/// @brief Sends data to the object store
///
/// @param oid ID of the object to send the data to
/// @param obj Object to send the data to
/// @param is_overwrite Signifies whether we are overwriting an existing object or writing to a new one
/// @param buf Buffer to hold the data being sent to the object store
/// @param offset Offset into the buffer to begin writing from (used for the multipart upload feature)
/// @param length Amount of data being sent in bytes
/// @param target_tier The tier to write the data to
///
/// @returns 0 on success, negative error code on error
int put(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    const bool is_overwrite,
    const void* buf,
    const std::size_t offset,
    const std::size_t length,
    const std::uint8_t target_tier = 0);

/// @brief Retrieves data from the object store
///
/// @param oid ID of the object to retrieve the data from
/// @param obj Object to retrieve the data from
/// @param buf Buffer to store the retrieved data
/// @param off Start offset for the data being read
/// @param len Amount of data being retrieved in bytes
/// @param src_tier Tier where the data is being read from
/// @param tgt_tier Identifier of the tier to copy/move the data to
///
/// @returns 0 on success, negative error code on error

int get(
    const struct hsm_uint oid,
    struct hsm_obj* obj,
    void* buf,
    const std::size_t off,
    const std::size_t len,
    const std::uint8_t src_tier = 0,
    const std::uint8_t tgt_tier = 0);

/// @brief Update an attribute for an object
///
/// @param oid ID of the object we are updating attributes for
/// @param attrs JSON string of attributes we wish to add/update
///
/// @returns Number of attributes updated on success, negative error code on error
int set_attrs(const struct hsm_uint& oid, const char* attrs);

/// @ brief Retrieve A list of attributes from the key-value store
///
/// @param oid ID of the object to retrieve the attributes from
/// @param attr_keys The attributes we wish to retrieve
///
/// @returns a string of the attributes retrieved in JSON format
std::string get_attrs(const struct hsm_uint& oid, const char* attr_keys);

/// @brief List all available attributes and their values for a given object
///
/// @param oid ID of the object to list attributes for
///
/// @returns JSON string of attributes for the object
std::string list_attrs(const struct hsm_uint& oid);

/// @brief remove an object and from the system, release all storage space and delete its data from the kvs
///
/// @param oid ID of the object to be removed
///
/// @returns 0 on success, negative error code on error
int remove(const struct hsm_uint& oid);

/// @brief List all objects on a given tier
///
/// @param tier Tier to list the objects on
///
/// @returns A vector of all found object IDs
std::vector<struct hsm_uint> list(const std::uint8_t tier = 0);

int copy(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier);


int move(
    const struct hsm_uint& oid,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier);

int release(const struct hsm_uint& oid, const std::uint8_t src_tier);

std::list<hsm_tier> get_tiers_info(std::vector<int> tids);

std::vector<uint8_t> list_tiers();

std::uint8_t locate(const struct hsm_uint& oid);
}  // namespace hestia
