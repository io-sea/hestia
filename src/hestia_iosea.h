#ifndef HESTIA_IOSEA_H_
#define HESTIA_IOSEA_H_

#include "hestia.h"

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hestia_create_mode_e {
    HESTIA_CREATE,  // Expect the object NOT to exist already when doing PUT
    HESTIA_UPDATE   // Expect the object to already exist when doing PUT
} hestia_create_mode_t;

typedef enum hestia_io_type_e {
    HESTIA_IO_EMPTY,      // Do a PUT with no data - no-op for GET
    HESTIA_IO_BUFFER,     // Do a PUT/GET with an in memory buffer
    HESTIA_IO_PATH,       // Do a PUT/GET with a path to a file
    HESTIA_IO_DESCRIPTOR  // Do a PUT/GET with an open file descriptor
} hestia_io_type_t;

/// @brief 128 bit identifier
///
/// A 128 bit identifier for an object divided into [hi, lo] 64 bit elements.
///
typedef struct HestiaId {
    uint64_t m_lo;  // The lower 64 bits
    uint64_t m_hi;  // The higher 64 bits
} HestiaId;

/// @brief IO Context for pasing data in PUT/GET operations
///
/// Used fields will depend on the IO transfer type
///

typedef struct HestiaIoContext {
    hestia_io_type_t m_type;  // Type of the transfer - see
                              // hestia_io_type_t docstring
    size_t m_offset;  // Offset into the object to insert the bytes - not
                      // widely supported in backends yet
    size_t m_length;  // The size of the transfer
    void* m_buffer;   // A buffer for memory based transfers. Use with
                      // HESTIA_IO_BUFFER
    char* m_path;     // A null-terminated path to a file on the system. Use
                      // with HESTIA_IO_PATH
    int m_fd;         // An open file descriptor to read/write from. Use with
                      // HESTIA_IO_DESCRIPTOR
} HestiaIoContext;

/// @brief Key-Value Pair helper struct
///
/// A Key-Value Pair helper struct
///

typedef struct HestiaKeyValuePair {
    char* m_key;
    char* m_value;
} HestiaKeyValuePair;

/// @brief Describes object data on a specific storage tier
///

typedef struct HestiaTierExtent {
    uint8_t m_tier_id;  // The id (0-255) of the storage tier
    size_t m_size;  // The size of the data on the tier (max offset + length)
    time_t m_creation_time;       // When the data was first added to the tier
    time_t m_last_modified_time;  // The last time the data on the tier was
                                  // modified
} HestiaTierExtent;

/// @brief Description of a storage object
///

typedef struct HestiaObject {
    char* m_name;            // Optional name - this can be an empty string '\0'
    size_t m_size;           // Largest size of the object across all tiers
    time_t m_creation_time;  // When the object was created
    time_t m_last_modified_time;  // The time of most recent data or metadata
                                  // update, on any tier.
    time_t m_last_accessed_time;  // The time of most recent data or
                                  // metadata read or update on any tier.
    HestiaTierExtent* m_tier_extents;  // List of data extents on each tier
    size_t m_num_tier_extents;
    HestiaKeyValuePair* m_attrs;  // List of USER provided metadata
    size_t m_num_attrs;
} HestiaObject;

/// @brief List all tiers in the system
///
/// Lists all registered tiers - whether or not they have any objects
///
/// @param tier_ids Will be populated as an array of ids for the tiers - free it with 'hestia_free_tier_ids()' when done.
/// @param num_ids Number of ids, i.e. size of the tier_ids array
/// @return 0 on success, hestia_error_e value on failure

int hestia_list_tiers(uint8_t** tier_ids, size_t* num_ids);

/// @brief Free an allocated tier_ids array
///
/// Free an allocated tier_ids array, as created by 'hestia_list_tiers()'.
///
/// @param tier_ids Array to be free'd
/// @return 0 on success, hestia_error_e value on failure

int hestia_free_tier_ids(uint8_t** tier_ids);

/// @brief Get data from the object with provided id
///
/// Get data from the object with provided id
///
/// @param object_id Id of object to get data from
/// @param io_context Details on the io setup
/// @param tier_id tier to get the data from
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_get(
    HestiaId* object_id, HestiaIoContext* io_context, uint8_t tier_id);

/// @brief Create or update an object - optionally add data
///
/// Create or update an object - optionally add data
///
/// @param object_id Id of object to create or update
/// @param create_mode specifies whether we are doing a create or update - to prevent accidental overwrites
/// @param io_context Details on the io setup - this can specify an 'empty' case for no data
/// @param tier_id tier to put the data to
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_put(
    HestiaId* object_id,
    hestia_create_mode_t create_mode,
    HestiaIoContext* io_context,
    uint8_t tier_id);

/// @brief Get object attributes
///
/// Get object attributes
///
/// @param object_id Id of object to get attributes for
/// @param object Object structure to be populated with attributes - call 'hestia_init_object()' when finished with it
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_get_attrs(HestiaId* object_id, HestiaObject* object);

/// @brief Reset an object attributes struct
///
/// Reset an object attributes struct - populated by
/// 'hestia_object_get_attrs()';
///
/// @param object Object structure to be (re)initialized
/// @return 0 on success, hestia_error_e value on failure

int hestia_init_object(HestiaObject* object);

/// @brief Set object USER attributes
///
/// You can set object USER attributes with this - it can't be used to override
/// SYSTEM attributes
///
/// @param object_id Id of object to set attributes on
/// @param key_value_pairs A populated array of key value pairs to set ob the object
/// @param num_key_pairs Number of key value pairs
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_set_attrs(
    HestiaId* object_id,
    const HestiaKeyValuePair key_value_pairs[],
    size_t num_key_pairs);

/// @brief Copy an object between tiers
///
/// Copy an object between tiers
///
/// @param object_id Id of object to copy
/// @param source_tier Tier to copy from
/// @param target_tier Tier to copy to
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_copy(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier);

/// @brief Move an object between tiers
///
/// Move an object between tiers - it will be removed from the source tier
///
/// @param object_id Id of object to move
/// @param source_tier Tier to move from (it will be removed from the tier in the operation)
/// @param target_tier Tier to move to
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_move(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier);

/// @brief Release an object from a tier
///
/// Release an object from a tier - optionally error if object only exists on
/// this tier
///
/// @param object_id Id of object to release
/// @param tier_id Tier to release from
/// @param error_if_last_tier If non-zero then error if object only exists on this tier
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_release(
    HestiaId* object_id, uint8_t tier_id, int error_if_last_tier);

/// @brief Entirely remove the object from the system
///
/// This includes removing all data from all tiers
///
/// @param object_id Id of object to remove
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_remove(HestiaId* object_id);

/// @brief List tiers containing this object
///
/// List tiers containing this object
///
/// @param object_id Id of object to list the tiers on
/// @param tier_ids array of ids which will be populated - free it with hestia_free_tier_ids() when finished
/// @param num_ids number of tier ids in the array
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_locate(
    HestiaId* object_id, uint8_t** tier_ids, size_t* num_ids);

/// @brief List all objects on a tier
///
/// List all objects on a tier
///
/// @param tier_id Tier to list objects on
/// @param object_ids array of ids which will be populated - free it with hestia_free_ids() when finished
/// @param num_ids number of object ids in the array
/// @return 0 on success, hestia_error_e value on failure

int hestia_object_list(uint8_t tier_id, HestiaId** object_ids, size_t* num_ids);

/// @brief Free a list of HestiaIds
///
/// Free a list of HestiaIds, e.g. from hestia_object_list()
///
/// @param ids Ids to be free'd
/// @return 0 on success, hestia_error_e value on failure

int hestia_free_ids(HestiaId** ids);

#ifdef __cplusplus
}
#endif

#endif
