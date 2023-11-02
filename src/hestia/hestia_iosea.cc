#include "hestia_iosea.h"

#include "Logger.h"
#include "StringUtils.h"
#include "Uuid.h"

#include "hestia_private.h"

#include <iostream>
#include <string>

namespace hestia {

extern "C" {

int hestia_init_object(HestiaObject* object)
{
    if (object->m_num_attrs > 0) {
        for (std::size_t idx = 0; idx < object->m_num_attrs; idx++) {
            delete[] object->m_attrs[idx].m_key;
            delete[] object->m_attrs[idx].m_value;
        }
        delete[] object->m_attrs;
        object->m_attrs = nullptr;
    }
    if (object->m_num_tier_extents > 0) {
        delete[] object->m_tier_extents;
        object->m_tier_extents = nullptr;
    }

    object->m_size               = 0;
    object->m_creation_time      = 0;
    object->m_last_modified_time = 0;
    object->m_num_tier_extents   = 0;
    object->m_num_attrs          = 0;
    delete[] object->m_name;
    object->m_name = nullptr;
    return 0;
}

int hestia_free_ids(HestiaId** ids)
{
    delete[] (*ids);
    *ids = nullptr;
    return 0;
}

int hestia_free_tier_ids(uint8_t** tier_ids)
{
    delete[] (*tier_ids);
    *tier_ids = nullptr;
    return 0;
}

int hestia_list_tiers(uint8_t** tier_ids, size_t* num_ids)
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

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));

    *num_ids  = attr_dict.get_sequence().size();
    *tier_ids = new uint8_t[*num_ids];

    std::size_t count = 0;
    for (const auto& item : attr_dict.get_sequence()) {
        const auto name    = item->get_map_item("name")->get_scalar();
        (*tier_ids)[count] = std::stoi(name);
        count++;
    }
    delete[] output;
    return 0;
}

int check_io_context(HestiaIoContext* io_context, bool)
{
    if (io_context == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        if (io_context->m_length > 0 && io_context->m_buffer == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
        }
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        if (io_context->m_path == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
        }
    }
    return 0;
}

int hestia_object_get(
    HestiaId* object_id, HestiaIoContext* io_context, uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }
    int rc = check_io_context(io_context, true);
    if (rc != 0) {
        return rc;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);

    char* activity_id{nullptr};
    int len_activity_id{0};
    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        rc = hestia_data_get(
            uuid.to_string().c_str(), io_context->m_buffer,
            &io_context->m_length, io_context->m_offset, tier_id, &activity_id,
            &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        rc = hestia_data_get_path(
            uuid.to_string().c_str(), io_context->m_path, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_DESCRIPTOR) {
        rc = hestia_data_get_descriptor(
            uuid.to_string().c_str(), io_context->m_fd, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }
    delete[] activity_id;
    return rc;
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

    if (output_size == 0) {
        return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));
    delete[] output;

    auto user_md_id = attr_dict.get_map_item("user_metadata")
                          ->get_map_item("id")
                          ->get_scalar();

    std::string input = "id=" + user_md_id + "\n";
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

void serialize_dict(
    const Dictionary& dict,
    HestiaKeyValuePair** key_value_pairs,
    size_t* num_key_pairs)
{
    if (dict.is_empty()) {
        *num_key_pairs = 0;
        return;
    }

    Map flattened_dict;
    dict.flatten(flattened_dict);

    *num_key_pairs    = flattened_dict.data().size();
    *key_value_pairs  = new HestiaKeyValuePair[*num_key_pairs];
    std::size_t count = 0;
    for (const auto& [key, value] : flattened_dict.data()) {
        StringUtils::to_char(key, &(*key_value_pairs)[count].m_key);
        StringUtils::to_char(value, &(*key_value_pairs)[count].m_value);
        count++;
    }
}

std::size_t get_int_map_item(const Dictionary& dict, const std::string& key)
{
    return std::stoull(dict.get_map_item(key)->get_scalar());
}

std::size_t get_extents_size(const Dictionary& tier_extent_dict)
{
    auto extents_dict = tier_extent_dict.get_map_item("extents");
    if (extents_dict->get_sequence().empty()) {
        return 0;
    }
    const auto& end_extent =
        extents_dict->get_sequence()[extents_dict->get_sequence().size() - 1];
    const auto offset = get_int_map_item(*end_extent, "offset");
    const auto length = get_int_map_item(*end_extent, "length");
    return offset + length;
}

int get_tier_ids(const std::string& uuids, std::vector<uint8_t>& ids)
{
    char* output{nullptr};
    int totals{0};
    int output_size{0};
    const auto rc = hestia_read(
        hestia_item_t::HESTIA_TIER, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, uuids.c_str(), uuids.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);

    if (rc != 0) {
        return rc;
    }

    Dictionary tier_attr_dict;
    tier_attr_dict.from_string(std::string(output, output_size));
    delete[] output;

    if (tier_attr_dict.get_type() == Dictionary::Type::SEQUENCE) {
        for (const auto& item : tier_attr_dict.get_sequence()) {
            const auto name = item->get_map_item("name")->get_scalar();
            if (!name.empty()) {
                ids.push_back(std::stoi(name));
            }
            else {
                ids.push_back(0);
            }
        }
    }
    else {
        const auto name = tier_attr_dict.get_map_item("name")->get_scalar();
        if (!name.empty()) {
            ids.push_back(std::stoi(name));
        }
        else {
            ids.push_back(0);
        }
    }
    return 0;
}

int hestia_object_get_attrs(HestiaId* object_id, HestiaObject* object)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
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

    if (output_size == 0) {
        return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));

    delete[] output;
    output = nullptr;

    object->m_creation_time = get_int_map_item(attr_dict, "creation_time");
    auto last_modified_time = get_int_map_item(attr_dict, "last_modified_time");

    const auto user_md_id = attr_dict.get_map_item("user_metadata")
                                ->get_map_item("id")
                                ->get_scalar();
    rc = hestia_read(
        hestia_item_t::HESTIA_USER_METADATA,
        hestia_query_format_t::HESTIA_QUERY_IDS, hestia_id_format_t::HESTIA_ID,
        0, 0, user_md_id.c_str(), user_md_id.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    Dictionary user_md_dict;
    user_md_dict.from_string(std::string(output, output_size));
    delete[] output;
    output = nullptr;

    const auto md_modified_time =
        get_int_map_item(user_md_dict, "last_modified_time");
    if (md_modified_time > last_modified_time) {
        last_modified_time = md_modified_time;
    }
    auto data = user_md_dict.get_map_item("data");
    if (data == nullptr) {
        object->m_num_attrs = 0;
    }
    else {
        serialize_dict(*data, &object->m_attrs, &object->m_num_attrs);
    }

    std::vector<HestiaTierExtent> extents;
    std::string tier_uuids;

    if (auto tiers = attr_dict.get_map_item("tiers"); tiers) {
        for (const auto& tier_extent_dict : tiers->get_sequence()) {
            const auto size = get_extents_size(*tier_extent_dict);
            if (size == 0) {
                continue;
            }

            const auto tier_id = tier_extent_dict->get_map_item("tier")
                                     ->get_map_item("id")
                                     ->get_scalar();
            tier_uuids += tier_id + "\n";
            HestiaTierExtent tier_extent;
            tier_extent.m_size = size;
            tier_extent.m_creation_time =
                get_int_map_item(*tier_extent_dict, "creation_time");
            tier_extent.m_last_modified_time =
                get_int_map_item(*tier_extent_dict, "last_modified_time");
            if (tier_extent.m_last_modified_time > last_modified_time) {
                last_modified_time = tier_extent.m_last_modified_time;
            }
            extents.push_back(tier_extent);
        }
    }

    std::size_t max_size{0};
    if (!extents.empty()) {
        std::vector<uint8_t> tier_ids;

        rc = get_tier_ids(tier_uuids, tier_ids);
        if (rc != 0) {
            return rc;
        }
        if (tier_ids.size() != extents.size()) {
            return -1;
        }

        for (std::size_t idx = 0; idx < extents.size(); idx++) {
            if (extents[idx].m_size > max_size) {
                max_size = extents[idx].m_size;
            }
            extents[idx].m_tier_id = tier_ids[idx];
        }

        object->m_tier_extents = new HestiaTierExtent[extents.size()];
        for (std::size_t idx = 0; idx < extents.size(); idx++) {
            object->m_tier_extents[idx] = extents[idx];
        }
        object->m_num_tier_extents = extents.size();
    }
    object->m_size               = max_size;
    object->m_last_modified_time = last_modified_time;
    if (auto name = attr_dict.get_map_item("name"); name) {
        StringUtils::to_char(name->get_scalar(), &object->m_name);
    }
    else {
        StringUtils::to_char("", &object->m_name);
    }
    return 0;
}

int create_if_not_existing(
    const std::string& object_id, hestia_create_mode_t create_mode)
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

    delete[] output;

    if (output_size == 0) {
        if (create_mode == hestia_create_mode_t::HESTIA_UPDATE) {
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
    else if (create_mode == hestia_create_mode_t::HESTIA_CREATE) {
        return hestia_error_e::HESTIA_ERROR_ATTEMPTED_OVERWRITE;
    }
    return 0;
}

int hestia_object_put(
    HestiaId* object_id,
    hestia_create_mode_t create_mode,
    HestiaIoContext* io_context,
    uint8_t tier_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (object_id == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    auto rc = check_io_context(io_context, false);
    if (rc != 0) {
        return rc;
    }

    Uuid uuid(object_id->m_lo, object_id->m_hi);
    const auto id_str = uuid.to_string();

    rc = create_if_not_existing(id_str, create_mode);
    if (rc != 0) {
        return rc;
    }

    char* activity_id{nullptr};
    int len_activity_id{0};

    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        rc = hestia_data_put(
            uuid.to_string().c_str(), io_context->m_buffer,
            io_context->m_length, io_context->m_offset, tier_id, &activity_id,
            &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        rc = hestia_data_put_path(
            uuid.to_string().c_str(), io_context->m_path, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_DESCRIPTOR) {
        rc = hestia_data_put_descriptor(
            uuid.to_string().c_str(), io_context->m_fd, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else {
        return 0;
    }
    if (rc != 0) {
        return rc;
    }

    delete[] activity_id;
    return 0;
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
    delete[] action_id;
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
    delete[] action_id;
    return rc;
}

int hestia_object_release(
    HestiaId* object_id, uint8_t tier_id, int error_if_last_tier)
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

    if (error_if_last_tier != 0) {
    }

    const auto rc = hestia_data_release(
        hestia_item_t::HESTIA_OBJECT, id_str.c_str(), tier_id, &action_id,
        &action_id_size);
    delete[] action_id;
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

    if (output_size == 0) {
        *num_ids = 0;
        return 0;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));
    delete[] output;

    auto extents = attr_dict.get_map_item("tiers");
    if (extents == nullptr) {
        *num_ids = 0;
        return 0;
    }

    std::string tier_uuids;
    for (const auto& tier_extent : extents->get_sequence()) {
        const auto size = get_extents_size(*tier_extent);
        if (size == 0) {
            continue;
        }

        const auto tier_id =
            tier_extent->get_map_item("tier")->get_map_item("id")->get_scalar();
        tier_uuids += tier_id + "\n";
    }

    if (tier_uuids.empty()) {
        *num_ids = 0;
        return 0;
    }
    std::vector<uint8_t> ids;
    rc = get_tier_ids(tier_uuids, ids);
    if (rc != 0) {
        return rc;
    }

    *num_ids  = ids.size();
    *tier_ids = new uint8_t[ids.size()];
    for (std::size_t idx = 0; idx < ids.size(); idx++) {
        (*tier_ids)[idx] = ids[idx];
    }
    return 0;
}

int hestia_object_list(uint8_t tier_id, HestiaId** object_ids, size_t* num_ids)
{
    // Query tier extents matching this tier name
    std::string filter = "tier_name=" + std::to_string(tier_id);

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    auto rc = hestia_read(
        hestia_item_t::HESTIA_TIER_EXTENT,
        hestia_query_format_t::HESTIA_QUERY_FILTER,
        hestia_id_format_t::HESTIA_NAME, 0, 0, filter.c_str(), filter.size(),
        hestia_io_format_t::HESTIA_IO_IDS, &output, &output_size, &totals);
    if (rc != 0) {
        return rc;
    }

    if (output_size == 0) {
        *num_ids = 0;
        return 0;
    }

    const auto output_str = std::string(output, output_size);
    delete[] output;

    std::cout << "query result is " << output_str << std::endl;

    // Get the object id from these extents
    Dictionary attr_dict;
    attr_dict.from_string(output_str);

    std::vector<std::string> object_id_strs;
    if (attr_dict.get_type() == Dictionary::Type::SEQUENCE) {
    }

    *num_ids = object_id_strs.size();
    (void)object_ids;
    return 0;
}
}

}  // namespace hestia