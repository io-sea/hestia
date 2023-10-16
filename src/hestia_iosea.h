#ifndef HESTIA_IOSEA_H_
#define HESTIA_IOSEA_H_

#include "hestia.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Start the Hestia client
///
/// This must be called before using any other functions. It should only be
/// called once, but can be called again after calling 'hestia_finish'.
///
/// @param config_path Optional null-terminated path to a config file. If null, default search paths are attempted.
/// @param token A null-terminated identifying token for a user or machine account. If user management is disabled or it is already provided in the config it can be null.
/// @param extra_config Extra config in JSON format, null-terminated. This will override anything in the config file. It can be null.
/// @return 0 on success, hestia_error_e value on failure

typedef struct HestiaId {
    uint64_t m_lo = 0;
    uint64_t m_hi = 0;
} HestiaId;

typedef struct HestiaKeyValuePair {
    char* m_key   = NULL;
    char* m_value = NULL;
} HestiaKeyValuePair;

int hestia_free_kv_pairs(HestiaKeyValuePair** key_value_pairs);

int hestia_free_ids(HestiaId** ids);

int hestia_list_tiers(uint8_t** tier_ids, int* num_ids);

int hestia_object_get(
    HestiaId* object_id,
    void* buffer,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_get_path(
    HestiaId* object_id,
    const char* path,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_get_descriptor(
    HestiaId* object_id,
    int fid,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_get_attrs(
    HestiaId* object_id,
    HestiaKeyValuePair** key_value_pairs,
    int* num_key_pairs);

int hestia_object_put(
    HestiaId* object_id,
    bool overwrite,
    const void* buffer,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_put_path(
    HestiaId* object_id,
    bool overwrite,
    const char* path,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_put_descriptor(
    HestiaId* object_id,
    bool overwrite,
    int fid,
    size_t offset,
    size_t length,
    uint8_t tier_id);

int hestia_object_set_attrs(
    HestiaId* object_id,
    const HestiaKeyValuePair* key_value_pairs,
    int num_key_pairs);

int hestia_object_copy(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier);

int hestia_object_locate(HestiaId* object_id, uint8_t** tier_ids, int* num_ids);

int hestia_object_list(uint8_t tier_id, HestiaId** object_ids, int* num_ids);

int hestia_object_move(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier);

int hestia_object_release(HestiaId* object_id, uint8_t tier_id);

int hestia_object_remove(HestiaId* object_id);

#ifdef __cplusplus
}
#endif

#endif
