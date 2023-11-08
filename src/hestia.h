#ifndef HESTIA_H_
#define HESTIA_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hestia_error_e {
    HESTIA_ERROR_OK = 0,               // Operation successful
    HESTIA_ERROR_NOT_FOUND,            // The requested resource wasn't found
    HESTIA_ERROR_ATTEMPTED_OVERWRITE,  // Attempted to overwrite a resource
                                       // while not permitted
    HESTIA_ERROR_BAD_STREAM,        // Failed to pass data through an I/O stream
    HESTIA_ERROR_BAD_INPUT_ID,      // Invalid input id format.
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
    HESTIA_ACTION,
    HESTIA_USER_METADATA,
    HESTIA_TIER_EXTENT,
    HESTIA_ITEM_TYPE_COUNT,
} hestia_item_t;

typedef enum hestia_id_format_e {
    HESTIA_ID_NONE = 0,
    HESTIA_ID      = 1 << 0,  // The unique ID for the item is given
    HESTIA_NAME = 1 << 1,  // A reader-friendly name for the item is given, e.g.
                           // my_object_123
    HESTIA_PARENT_ID = 1 << 2,  // The unique ID for the parent item is given
    HESTIA_PARENT_NAME =
        1 << 3  // A reader-friendly name for the parent item is given
} hestia_id_format_t;

typedef enum hestia_io_format_e {
    HESTIA_IO_NONE = 0,
    HESTIA_IO_IDS  = 1 << 0,  // IDs will be provided in I/O - does not combine
                              // with JSON option
    HESTIA_IO_JSON = 1 << 1,  // I/O will be in JSON format - does not combine
                              // with other options
    HESTIA_IO_KEY_VALUE = 1 << 2,  // I/O will be in Key-Value format
} hestia_io_format_t;

typedef enum hestia_query_format_e {
    HESTIA_QUERY_NONE = 0,
    HESTIA_QUERY_IDS,     // The query will be a sequence of unique item ids
    HESTIA_QUERY_FILTER,  // The query will be a key-value sequence to be
                          // matched by specific items
    HESTIA_QUERY_TYPE_COUNT,
} hestia_query_format_t;

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

/// @brief Free a Hestia provided output buffer
///
/// Free a Hestia provided output buffer, this just calls 'delete[] * output;'
/// in practice.
/// @return 0 on success, hestia_error_e value on failure
int hestia_free_output(char** output);

/// @brief Creates a new item in the Hestia system and returns its ID
///
/// @param subject The type of item to be created, for example a Storage Object (HESTIA_OBJECT)
/// @param input_format A format specifier for the content of the 'input' buffer
/// @param id_format A format specifier for provided IDs - this allows a range of ways to address items
/// @param input An optional input buffer - content is formatted according to 'input_format', for example it
/// might be a JSON string, or a sequence of ID strings followed by Key-Value
/// pairs. It can be empty for a default created object.
/// @param len_input Size of the provided input buffer
/// @param output_format Format specifier for requested output - for example you can request just an ID for the created
/// item or its full JSON representation. The buffer will be allocated by Hestia
/// and should be free'd when finished with 'hestia_finish()'
/// @param len_output Length of the output buffer - will be returned by Hestia.
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_create(
    hestia_item_t subject,
    hestia_io_format_t input_format,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** output,
    int* len_output);

/// @brief Updates an existing item in Hestia
///
/// @param subject The type of item to be updated, for example a Storage Object (HESTIA_OBJECT)
/// @param input_format A format specifier for the content of the 'input' buffer
/// @param id_format A format specifier for provided IDs - this allows a range of ways to address items
/// @param input An input buffer - content is formatted according to 'input_format', for example it
/// might be a JSON string, or a sequence of ID strings followed by Key-Value
/// pairs.
/// @param len_input Size of the provided input buffer
/// @param output_format Format specifier for requested output - for example you can request just an ID for the updated
/// item or its full JSON representation. The buffer will be allocated by Hestia
/// and should be free'd when finished with 'hestia_finish()'
/// @param len_output Length of the output buffer - will be returned by Hestia.
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_update(
    hestia_item_t subject,
    hestia_io_format_t input_format,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** output,
    int* len_output);

/// @brief Read the content of an item if it exists
///
/// @param subject The type of item to be read, for example a Storage Object (HESTIA_OBJECT)
/// @param query_format A format specifier for the type of query, e.g. searching with an id or by attributes
/// @param id_format A format specifier for provided IDs - this allows a range of ways to address items
/// @param offset Intended for paging but not currently implemented
/// @param count Intended for paging but not currently implemented
/// @param input An input buffer for the query, formatted according to query_format
/// @param len_input Size of the provided input buffer
/// @param output_format Format specifier for requested output - for example you can request just an ID for the updated
/// item or its full JSON representation. The buffer will be allocated by Hestia
/// and should be free'd when finished with 'hestia_finish()'
/// @param len_output Length of the output buffer - will be returned by Hestia.
/// @param total_count Intended for paging but not currently implemented
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_read(
    hestia_item_t subject,
    hestia_query_format_t query_format,
    hestia_id_format_t id_format,
    int offset,
    int count,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** output,
    int* len_output,
    int* total_count);

/// @brief remove an item entirely from the system
///
/// @param subject The type of item to be remove, for example a Storage Object (HESTIA_OBJECT)
/// @param id_format A format specifier for provided IDs - this allows a range of ways to address items
/// @param input An input buffer for the addressing query
/// @param len_input Size of the provided input buffer

/// @return 0 on success, hestia_error_e value on failure
int hestia_remove(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input);

/// @brief Intended to check object existence - not currently implemented
int hestia_identify(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    const char* output,
    int* len_output,
    int* exists);

/// @brief Puts data to the object store
///
/// @param oid ID of the object, should be null-terminated.
/// @param buf Buffer to with the data being sent to the object store
/// @param length Size of the buffer in bytes
/// @param offset Offset into the object to begin writing to.
/// @param tier The storage tier to write the data to
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_put(
    const char* oid,
    const void* buf,
    const size_t length,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Puts data to the object store via a file descriptor
///
/// @param oid ID of the object, should be null-terminated.
/// @param file_discriptor A file descriptor from an 'open()' call. It may be to a file on disk or pipe/socket etc.
/// @param length Amount of data to read from the descriptor
/// @param offset Offset into object to begin writing to
/// @param tier The storage tier to write the data to
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_put_descriptor(
    const char* oid,
    int file_discriptor,
    const size_t length,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Puts data to the object store via a path to file on the system
///
/// @param oid ID of the object, should be null-terminated.
/// @param path Path to the file to be added, path should be null-terminated.
/// @param length Amount of data to read from the descriptor
/// @param offset Offset into object to begin writing to
/// @param tier The storage tier to write the data to
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_put_path(
    const char* oid,
    const char* path,
    const size_t length,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Retrieves data from the object store
///
/// @param oid ID of the object, should be null-terminated.
/// @param buf Buffer to store the retrieved data
/// @param len Amount of data being retrieved in bytes
/// @param off Start offset into the object for the data being read
/// @param tier Tier where the data is being read from
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure

int hestia_data_get(
    const char* oid,
    void* buf,
    size_t* len,
    const size_t off,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Retrieves data from the object store and writes to an open fd
///
/// @param oid ID of the object, should be null-terminated.
/// @param file_discriptor Open file descriptor to write data to
/// @param len Amount of data being retrieved in bytes
/// @param off Start offset into the object for the data being read
/// @param tier Tier where the data is being read from
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_get_descriptor(
    const char* oid,
    int file_discriptor,
    const size_t len,
    const size_t off,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Retrieves data from the object store and writes to a path
///
/// @param oid ID of the object, should be null-terminated.
/// @param path A path to write data to
/// @param len Amount of data being retrieved in bytes
/// @param off Start offset into the object for the data being read
/// @param tier Tier where the data is being read from
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_get_path(
    const char* oid,
    const char* path,
    const size_t len,
    const size_t off,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Copy an object from one tier to another
///
/// @param subject The type of item to be copied, for example a Storage Object (HESTIA_OBJECT)
/// @param oid ID of the object, should be null-terminated.
/// @param src_tier Tier the object is to be copies from
/// @param tgt_tier Tier the object will be copied to
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_copy(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Move an object from one tier to another
///
/// @param subject The type of item to be moved, for example a Storage Object (HESTIA_OBJECT)
/// @param oid ID of the object, should be null-terminated.
/// @param src_tier Tier the object is to be moved from
/// @param tgt_tier Tier the object will be moved to
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_move(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Remove an objects data from a given tier
///
/// @param subject The type of item to be released, for example a Storage Object (HESTIA_OBJECT)
/// @param oid ID of the object, should be null-terminated.
/// @param tier Tier the object is to be removed from
/// @param activity_id Will be populated with an activity id, which can be queried for status and progress.
/// It will be allocated by Hestia and should be free'd with: 'hestia_finish()'.
/// @param len_activity_id Size of the buffer allocated by Hestia for the activity Id
///
/// @return 0 on success, hestia_error_e value on failure
int hestia_data_release(
    hestia_item_t subject,
    const char* id,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id);

/// @brief Add the last set error message to the preallocated buffer
void hestia_get_last_error(char* message_buffer, size_t len_message_buffer);

#ifdef __cplusplus
}
#endif

#endif
