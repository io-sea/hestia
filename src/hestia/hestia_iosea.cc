#include "hestia_iosea.h"

#include "StringAdapter.h"
#include "Uuid.h"
#include "hestia_private.h"

#include <string>

namespace hestia {
int hestia_free_kv_pairs(HestiaKeyValuePair** key_value_pairs)
{
    (void)key_value_pairs;
    return 0;
}

int hestia_free_ids(HestiaId** ids)
{
    (void)ids;
    return 0;
}

int hestia_list_tiers(uint8_t** tier_ids, int* num_ids)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    const auto rc = hestia_read(
        hestia_item_t::HESTIA_TIER, hestia_query_format_t::HESTIA_QUERY_NONE,
        hestia_id_format_t::HESTIA_ID_NONE, 0, 0, nullptr, 0,
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    KeyValueAdapter attr_adapter;
    Dictionary attr_dict;
    attr_adapter.dict_from_string(std::string(output, output_size), attr_dict);

    *num_ids  = attr_dict.get_sequence().size();
    *tier_ids = new uint8_t[*num_ids];

    std::size_t count = 0;
    for (const auto& item : attr_dict.get_sequence()) {
        const auto name    = item->get_map_item("name")->get_scalar();
        (*tier_ids)[count] = std::stoi(name);
        count++;
    }
    delete output;
    return 0;
}

int hestia_object_get(
    HestiaId* object_id,
    void* buffer,
    size_t offset,
    size_t length,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    if (buffer == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);

    char* activity_id{nullptr};
    int len_activity_id{0};
    const auto rc = hestia_data_get(
        uuid.to_string().c_str(), buffer, &length, offset, tier_id,
        &activity_id, &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

int hestia_object_get_path(
    HestiaId* object_id,
    const char* path,
    size_t offset,
    size_t length,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);

    char* activity_id{nullptr};
    int len_activity_id{0};
    const auto rc = hestia_data_get_path(
        uuid.to_string().c_str(), path, length, offset, tier_id, &activity_id,
        &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

int hestia_object_get_descriptor(
    HestiaId* object_id, int fid, size_t offset, size_t length, uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);

    char* activity_id{nullptr};
    int len_activity_id{0};
    const auto rc = hestia_data_get_descriptor(
        uuid.to_string().c_str(), fid, length, offset, tier_id, &activity_id,
        &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

void str_to_char(const std::string& str, char** chars)
{
    *chars = new char[str.size() + 1];
    for (std::size_t idx = 0; idx < str.size(); idx++) {
        (*chars)[idx] = str[idx];
    }
    (*chars)[str.size()] = '\0';
}

int hestia_object_get_attrs(
    HestiaId* object_id,
    HestiaKeyValuePair** key_value_pairs,
    size_t* num_key_pairs)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    const auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, id_str.c_str(), id_str.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    KeyValueAdapter attr_adapter;
    Dictionary attr_dict;
    attr_adapter.dict_from_string(std::string(output, output_size), attr_dict);

    if (attr_dict.get_sequence().empty()) {
        return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
    }

    auto user_md_dict =
        attr_dict.get_sequence()[0]->get_map_item("user_metadata");
    Map flattened_dict;
    user_md_dict->flatten(flattened_dict);

    *num_key_pairs = flattened_dict.data().size();

    *key_value_pairs = new HestiaKeyValuePair[*num_key_pairs];

    std::size_t count = 0;
    for (const auto& [key, value] : flattened_dict.data()) {
        str_to_char(key, &(*key_value_pairs)[count].m_key);
        str_to_char(value, &(*key_value_pairs)[count].m_value);
        count++;
    }
    delete output;
    return 0;
}

int create_if_not_existing(const std::string& object_id, bool overwrite)
{
    char* output{nullptr};
    int totals{0};
    int output_size{0};

    auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, object_id.c_str(),
        object_id.size(), hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output,
        &output_size, &totals);
    if (rc != 0) {
        return rc;
    }

    KeyValueAdapter attr_adapter;
    Dictionary attr_dict;
    attr_adapter.dict_from_string(std::string(output, output_size), attr_dict);
    delete output;

    if (attr_dict.get_sequence().empty()) {
        if (overwrite) {
            return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
        }

        rc = hestia_create(
            hestia_item_t::HESTIA_OBJECT, hestia_io_format_t::HESTIA_IO_IDS,
            hestia_id_format_t::HESTIA_ID, object_id.c_str(), object_id.size(),
            hestia_io_format_t::HESTIA_IO_NONE, nullptr, 0);
        if (rc != 0) {
            return rc;
        }
    }
    else if (!overwrite) {
        return hestia_error_e::HESTIA_ERROR_ATTEMPTED_OVERWRITE;
    }
    return 0;
}

int hestia_object_put(
    HestiaId* object_id,
    bool overwrite,
    const void* buffer,
    size_t offset,
    size_t length,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    auto rc = create_if_not_existing(id_str, overwrite);
    if (rc != 0) {
        return rc;
    }

    if (buffer == nullptr) {
        return 0;
    }

    char* activity_id{nullptr};
    int len_activity_id{0};
    rc = hestia_data_put(
        uuid.to_string().c_str(), buffer, length, offset, tier_id, &activity_id,
        &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

int hestia_object_put_path(
    HestiaId* object_id,
    bool overwrite,
    const char* path,
    size_t offset,
    size_t length,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    auto rc = create_if_not_existing(id_str, overwrite);
    if (rc != 0) {
        return rc;
    }

    if (path == nullptr) {
        return 0;
    }

    char* activity_id{nullptr};
    int len_activity_id{0};
    rc = hestia_data_put_path(
        uuid.to_string().c_str(), path, length, offset, tier_id, &activity_id,
        &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

int hestia_object_put_descriptor(
    HestiaId* object_id,
    bool overwrite,
    int fid,
    size_t offset,
    size_t length,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    auto rc = create_if_not_existing(id_str, overwrite);
    if (rc != 0) {
        return rc;
    }

    char* activity_id{nullptr};
    int len_activity_id{0};
    rc = hestia_data_put_descriptor(
        uuid.to_string().c_str(), fid, length, offset, tier_id, &activity_id,
        &len_activity_id);
    if (rc != 0) {
        return rc;
    }
    delete activity_id;
    return 0;
}

int hestia_object_set_attrs(
    HestiaId* object_id,
    const HestiaKeyValuePair* key_value_pairs,
    size_t num_key_pairs)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, id_str.c_str(), id_str.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    KeyValueAdapter attr_adapter;
    Dictionary attr_dict;
    attr_adapter.dict_from_string(std::string(output, output_size), attr_dict);
    if (attr_dict.get_sequence().empty()) {
        return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
    }

    auto user_md_id = attr_dict.get_map_item("user_metadata")
                          ->get_map_item("id")
                          ->get_scalar();

    std::string input;
    for (std::size_t idx = 0; idx < num_key_pairs; idx++) {
        input += "data." + std::string(key_value_pairs[idx].m_key) + "="
                 + std::string(key_value_pairs[idx].m_value) + "\n";
    };

    rc = hestia_update(
        hestia_item_t::HESTIA_USER_METADATA,
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, hestia_id_format_t::HESTIA_ID,
        input.c_str(), input.size(), hestia_io_format_t::HESTIA_IO_NONE,
        nullptr, 0);
    return rc;
}

int hestia_object_copy(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();
    char* action_id{nullptr};
    int action_id_size{0};
    const auto rc = hestia_data_copy(
        hestia_item_t::HESTIA_OBJECT, id_str.c_str(), source_tier, target_tier,
        &action_id, &action_id_size);
    delete action_id;
    return rc;
}

int hestia_object_move(
    HestiaId* object_id, uint8_t source_tier, uint8_t target_tier)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();
    char* action_id{nullptr};
    int action_id_size{0};
    const auto rc = hestia_data_move(
        hestia_item_t::HESTIA_OBJECT, id_str.c_str(), source_tier, target_tier,
        &action_id, &action_id_size);
    delete action_id;
    return rc;
}

int hestia_object_release(HestiaId* object_id, uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();
    char* action_id{nullptr};
    int action_id_size{0};
    const auto rc = hestia_data_release(
        hestia_item_t::HESTIA_OBJECT, id_str.c_str(), tier_id, &action_id,
        &action_id_size);
    delete action_id;
    return rc;
}

int hestia_object_remove(HestiaId* object_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    const auto rc = hestia_remove(
        hestia_item_t::HESTIA_OBJECT, hestia_id_format_t::HESTIA_ID,
        id_str.c_str(), id_str.size());
    return rc;
}

int hestia_object_locate(
    HestiaId* object_id, uint8_t** tier_ids, size_t* num_ids)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, id_str.c_str(), id_str.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    KeyValueAdapter attr_adapter;
    Dictionary attr_dict;
    attr_adapter.dict_from_string(std::string(output, output_size), attr_dict);
    if (attr_dict.get_sequence().empty()) {
        *num_ids = 0;
        return 0;
    }

    auto extents = attr_dict.get_map_item("tiers");

    std::string tier_uuids;
    for (const auto& tier_extent : extents->get_sequence()) {
        if (tier_extent->get_map_item("extents")->get_sequence().empty()) {
            continue;
        }
        else if (
            tier_extent->get_map_item("extents")->get_sequence().size() == 1) {
            if (tier_extent->get_map_item("extents")
                    ->get_sequence()[0]
                    ->get_map_item("length")
                    ->get_scalar()
                == "0") {
                continue;
            }
        }
        const auto tier_id =
            tier_extent->get_map_item("tier")->get_map_item("id")->get_scalar();
        tier_uuids += tier_id + "/n";
    }

    if (tier_uuids.empty()) {
        *num_ids = 0;
        return 0;
    }

    rc = hestia_read(
        hestia_item_t::HESTIA_TIER, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID_NONE, 0, 0, tier_uuids.c_str(),
        tier_uuids.size(), hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output,
        &output_size, &totals);
    if (rc != 0) {
        return rc;
    }

    Dictionary tier_attr_dict;
    attr_adapter.dict_from_string(
        std::string(output, output_size), tier_attr_dict);

    *num_ids  = tier_attr_dict.get_sequence().size();
    *tier_ids = new uint8_t[*num_ids];

    std::size_t count = 0;
    for (const auto& item : tier_attr_dict.get_sequence()) {
        const auto name    = item->get_map_item("name")->get_scalar();
        (*tier_ids)[count] = std::stoi(name);
        count++;
    }
    return 0;
}

int hestia_object_list(uint8_t tier_id, HestiaId** object_ids, size_t* num_ids)
{
    // Convert the 'tier_id' to tier unique identifier

    // Query tier extents matching this unique identifier

    // Get the object id from these extents

    (void)tier_id;
    (void)object_ids;
    (void)num_ids;
    return 0;
}

}  // namespace hestia