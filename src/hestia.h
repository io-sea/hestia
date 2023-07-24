#ifndef HESTIA_H_
#define HESTIA_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hestia_error_e {
    /// No error
    HESTIA_ERROR_OK = 0,               // Operation successful
    HESTIA_ERROR_NOT_FOUND,            // The requested resource wasn't found
    HESTIA_ERROR_ATTEMPTED_OVERWRITE,  // Attempted to overwrite a resource
                                       // while not permitted
    HESTIA_ERROR_BAD_STREAM,        // Failed to pass data through an I/O stream
    HESTIA_ERROR_BAD_INPUT_ID,      // Invalid input UUID format.
    HESTIA_ERROR_BAD_INPUT_BUFFER,  // Invalid input buffer.
    HESTIA_ERROR_CLIENT_STATE,      // Hestia client in unexpected state
    HESTIA_ERROR_UNKNOWN,           // Catch-all for unspecified errors
    HESTIA_ERROR_COUNT,
} hestia_error_t;

typedef enum hestia_item_e {
    HESTIA_OBJECT = 0,
    HESTIA_TIER,
    HESTIA_DATASET,
    HESTIA_USER,
    HESTIA_NODE,
    HESTIA_ITEM_TYPE_COUNT,
} hestia_item_t;

typedef enum hestia_id_format_e {
    HESTIA_ID_NONE     = 0,
    HESTIA_ID          = 1 << 0,
    HESTIA_NAME        = 1 << 1,
    HESTIA_PARENT_ID   = 1 << 2,
    HESTIA_PARENT_NAME = 1 << 3
} hestia_id_format_t;

typedef enum hestia_attribute_format_e {
    HESTIA_ATTRS_NONE = 0,
    HESTIA_ATTRS_JSON,
    HESTIA_ATTRS_KEY_VALUE,
    HESTIA_ATTRS_TYPE_COUNT,
} hestia_attribute_format_t;

typedef enum hestia_output_format_e {
    HESTIA_OUTPUT_NONE            = 0,
    HESTIA_OUTPUT_IDS             = 1 << 0,
    HESTIA_OUTPUT_ATTRS_JSON      = 1 << 1,
    HESTIA_OUTPUT_ATTRS_KEY_VALUE = 1 << 2,
} hestia_output_format_t;

typedef enum hestia_query_format_e {
    HESTIA_QUERY_NONE = 0,
    HESTIA_QUERY_IDS,
    HESTIA_QUERY_INDEX,
    HESTIA_QUERY_TYPE_COUNT,
} hestia_query_format_t;

#define HESTIA_UUID_SIZE 37
#define HESTIA_MAX_ITEMS 1000
#define HESTIA_MAX_ATTRS 1000

/// @brief Start the Hestia client
///
/// This must be called before using any other functions. It should only be
/// called once, but can be called again after calling 'hestia_finish'.
///
/// @param config_path Optional null-terminated path to a config file. If null, default search paths are attempted.
/// @param token A null-terminated identifying token for a user or machine account. If user management is disabled or it is already provided in the config it can be null.
/// @param extra_config Extra config in JSON format, null-terminated. This will override anything in the config file. It can be null.
/// @return 0 on success, hestia_error_e value on failure

int hestia_initialize(
    const char* config_path, const char* token, const char* extra_config);

/// @brief Terminate the Hestia client
///
/// This should be called when the Hestia sessions finishes. 'hestia_initialize'
/// can be called afterward again if needed.
///
/// @return 0 on success, hestia_error_e value on failure
///
int hestia_finish();

/// @brief Creates a new object in the Hestia system and returns an identifier in UUID format
///
/// @param oid Buffer to hold the returned UUID in format 'ffffffff-ffff-835a-ffff-ffffffffff9c'. It should be sized to hold the uuid and null-temrinator - HESTIA_UUID_SIZE=37 bytes.
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_create(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    hestia_attribute_format_t attribute_format,
    hestia_output_format_t output_format,
    const char* input,
    int len_input,
    char** response,
    int* len_response);

/// @brief Get the attributes of the object as key-value pairs
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param keys An array of fetched key entries of size num_attrs, each will be null-terminated. Should be pre-allocated to HESTIA_MAX_ATTRS and free'd when finished.
/// @param values An array of fetched value entries of size num_attrs, each will be null-terminated. Should be pre-allocated to HESTIA_MAX_ATTRS and free'd when finished.
/// @param num_attrs Number of attributes to get - size of the key entries array
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_read(
    hestia_item_t subject,
    hestia_query_format_t query_format,
    hestia_id_format_t id_format,
    hestia_output_format_t output_format,
    int offset,
    int count,
    const char* query,
    int len_query,
    char** response,
    int* len_response,
    int* total_count);

/// @brief Set or update attributes for an object.
///
/// Attributes are provided as a pair of key-value arrays of the same size with
/// null-terminated entries.
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param keys An array of key entries of size num_attrs, each should be null-terminated
/// @param values An array of corresponding value entries of size num_attrs, each should be null-terminated.
/// @param num_attrs Number of attributes to set
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_update(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    hestia_attribute_format_t attribute_format,
    hestia_output_format_t output_format,
    const char* input,
    int len_input,
    char** response,
    int* len_response);

/// @brief remove an object entirely from the system
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_remove(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input);

/// @brief Puts data to the object store
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param buf Buffer to with the data being sent to the object store
/// @param length Size of the buffer in bytes
/// @param offset Offset into the object to begin writing to.
/// @param tier The storage tier to write the data to
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_put(
    const char* oid,
    const void* buf,
    const size_t length,
    const size_t offset,
    const uint8_t tier);

/// @brief Puts data to the object store via a file descriptor
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param file_discriptor A file descriptor from an 'open()' call. It may be to a file on disk or pipe/socket etc.
/// @param length Amount of data to read from the descriptor
/// @param offset Offset into object to begin writing to
/// @param tier The storage tier to write the data to
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_put_descriptor(
    const char* oid,
    int file_discriptor,
    const size_t length,
    const size_t offset,
    const uint8_t tier);

/// @brief Retrieves data from the object store
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param buf Buffer to store the retrieved data
/// @param len Amount of data being retrieved in bytes
/// @param off Start offset into the object for the data being read
/// @param tier Tier where the data is being read from
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_get(
    const char* oid,
    void* buf,
    const size_t len,
    const size_t off,
    const uint8_t tier);

/// @brief Copy an object from one tier to another
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param src_tier Tier the object is to be copies from
/// @param tgt_tier Tier the object will be copied to
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_copy(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier);

/// @brief Move an object from one tier to another
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param src_tier Tier the object is to be moved from
/// @param tgt_tier Tier the object will be moved to
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_move(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier);

/// @brief Remove an objects data from a given tier
///
/// @param oid ID of the object. Should be in UUID format 'ffffffff-ffff-835a-ffff-ffffffffff9c' with a null-terminator.
/// @param tier Tier the object is to be removed from
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_release(
    hestia_item_t subject, const char* id, const uint8_t tier);

#ifdef __cplusplus
}
#endif

#endif
