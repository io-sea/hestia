#include "hestia_iosea.h"

#include "ErrorUtils.h"
#include "Logger.h"
#include "StringUtils.h"
#include "Uuid.h"

#include "hestia_private.h"

#include <iostream>
#include <set>
#include <string>

namespace hestia {

void set_last_error(const std::string& msg)
{
    LOG_ERROR(msg);
    HestiaPrivate::get_client()->set_last_error(msg);
}

int check_object_id(HestiaId* object_id, bool should_be_init)
{
    if (object_id == nullptr) {
        set_last_error("Passed in null object id");
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    if (should_be_init && object_id->m_user_initialized != 2) {
        set_last_error(
            "HestiaId is not initialized - possible missing call to: hestia_init_id()");
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }
    return 0;
}

int validate_uuid(HestiaId* object_id)
{
    if (!HestiaPrivate::get_client()->is_id_validation_active()) {
        return 0;
    }

    if (object_id->m_uuid != nullptr) {
        try {
            Uuid::from_string(object_id->m_uuid);
        }
        catch (const std::exception& e) {
            set_last_error(
                "Uuid not in expected format - should look like: 'xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx'");
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
        }
    }
    return 0;
}

std::size_t get_int_map_item(const Dictionary& dict, const std::string& key)
{
    if (dict.has_map_item(key)) {
        return std::stoull(dict.get_map_item(key)->get_scalar());
    }
    else {
        return 0;
    }
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
            if (!item->has_map_item("priority")) {
                set_last_error(
                    "Tier is missing a 'priority' - possible config issue.");
                return -1;
            }

            const auto priority = item->get_map_item("priority")->get_scalar();
            if (!priority.empty()) {
                ids.push_back(std::stoi(priority));
            }
            else {
                ids.push_back(0);
            }
        }
    }
    else {
        if (!tier_attr_dict.has_map_item("priority")) {
            set_last_error(
                "Tier is missing a 'priority' - possible config issue.");
            return -1;
        }
        const auto priority =
            tier_attr_dict.get_map_item("priority")->get_scalar();
        if (!priority.empty()) {
            ids.push_back(std::stoi(priority));
        }
        else {
            ids.push_back(0);
        }
    }
    return 0;
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

std::pair<int, std::string> create_if_not_existing(
    HestiaId* object_id, hestia_create_mode_t create_mode)
{
    char* output{nullptr};
    int output_size{0};
    int totals{0};

    std::string id;
    if (object_id->m_user_initialized == 2) {
        if (object_id->m_uuid == nullptr) {
            id = Uuid(object_id->m_lo, object_id->m_hi).to_string();
        }
        else {
            if (auto rc = validate_uuid(object_id); rc != 0) {
                return {rc, {}};
            }
            id = object_id->m_uuid;
        }

        if (auto rc = hestia_read(
                hestia_item_t::HESTIA_OBJECT,
                hestia_query_format_t::HESTIA_QUERY_IDS,
                hestia_id_format_t::HESTIA_ID, 0, 0, id.c_str(), id.size(),
                hestia_io_format_t::HESTIA_IO_IDS, &output, &output_size,
                &totals);
            rc != 0) {
            return {rc, {}};
        }
        delete[] output;
        output = nullptr;
    }

    if (output_size == 0) {
        if (create_mode == hestia_create_mode_t::HESTIA_UPDATE) {
            return {hestia_error_e::HESTIA_ERROR_NOT_FOUND, {}};
        }
        auto rc = hestia_create(
            hestia_item_t::HESTIA_OBJECT, hestia_io_format_t::HESTIA_IO_IDS,
            hestia_id_format_t::HESTIA_ID, id.c_str(), id.size(),
            hestia_io_format_t::HESTIA_IO_IDS, &output, &output_size);
        if (rc != 0) {
            return {rc, {}};
        }
        if (id.empty()) {
            id = std::string(output, output_size);
        }
        delete[] output;
    }
    else if (create_mode == hestia_create_mode_t::HESTIA_CREATE) {
        set_last_error(
            SOURCE_LOC() + " | Attempted to overwrite existing object.");
        return {hestia_error_e::HESTIA_ERROR_ATTEMPTED_OVERWRITE, {}};
    }
    return {0, id};
}

extern "C" {

int hestia_init_id(HestiaId* id)
{
    id->m_lo               = 0;
    id->m_hi               = 0;
    id->m_uuid             = nullptr;
    id->m_user_initialized = 2;
    return 0;
}

int hestia_free_id(HestiaId* id)
{
    delete[] id->m_uuid;
    id->m_uuid = nullptr;
    return 0;
}

int hestia_init_io_ctx(HestiaIoContext* io_ctx)
{
    io_ctx->m_type   = hestia_io_type_t::HESTIA_IO_EMPTY;
    io_ctx->m_offset = 0;
    io_ctx->m_length = 0;
    io_ctx->m_buffer = nullptr;
    io_ctx->m_path   = nullptr;
    io_ctx->m_fd     = -1;
    return 0;
}

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

    object->m_size             = 0;
    object->m_creation_time    = 0;
    object->m_mtime            = 0;
    object->m_ctime            = 0;
    object->m_num_tier_extents = 0;
    object->m_num_attrs        = 0;
    delete[] object->m_name;
    object->m_name = nullptr;
    delete[] object->m_uuid;
    object->m_uuid = nullptr;
    return 0;
}

int hestia_init_tier(HestiaTier* tier)
{
    tier->m_index            = 0;
    tier->m_user_initialized = 2;
    return 0;
}

int hestia_free_ids(HestiaId** ids, size_t num_ids)
{
    for (std::size_t idx = 0; idx < num_ids; idx++) {
        delete[] ids[idx]->m_uuid;
    }
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

int check_io_context(HestiaIoContext* io_context)
{
    if (io_context == nullptr) {
        set_last_error("Provided io_context is null");
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        if (io_context->m_length > 0 && io_context->m_buffer == nullptr) {
            set_last_error("Provided io_buffer is null");
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
        }
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        if (io_context->m_path == nullptr) {
            set_last_error("Provided path is null");
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
        }
    }
    return 0;
}

int hestia_object_get(
    HestiaId* object_id, HestiaIoContext* io_context, HestiaTier* tier)
{
    // NOLINTBEGIN
    if (!HestiaPrivate::check_initialized()) {
        set_last_error("Missing call to hestia_initialize()");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    if (auto rc = check_object_id(object_id, true); rc != 0) {
        return rc;
    }
    if (auto rc = check_io_context(io_context); rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }

    uint8_t tier_id{255};
    if (tier->m_user_initialized == 2) {
        tier_id = tier->m_index;
    }
    else {
        uint8_t* tier_ids{nullptr};
        std::size_t num_tier_ids{0};
        auto rc = hestia_object_locate(object_id, &tier_ids, &num_tier_ids);
        if (rc != 0) {
            return rc;
        }
        if (num_tier_ids > 0) {
            for (std::size_t idx = 0; idx < num_tier_ids; idx++) {
                if (tier_ids && tier_ids[idx] < tier_id) {
                    tier_id = tier_ids[idx];
                }
            }
            hestia_free_tier_ids(&tier_ids);
        }
    }

    char* activity_id{nullptr};
    int len_activity_id{0};
    int rc{0};
    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        rc = hestia_data_get(
            uuid.c_str(), io_context->m_buffer, &io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        rc = hestia_data_get_path(
            uuid.c_str(), io_context->m_path, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_DESCRIPTOR) {
        rc = hestia_data_get_descriptor(
            uuid.c_str(), io_context->m_fd, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else {
        set_last_error("Unsupported input buffer type");
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }
    delete[] activity_id;
    return rc;
}

int hestia_object_get_attrs(HestiaId* object_id, HestiaObject* object)
{
    if (!HestiaPrivate::check_initialized()) {
        set_last_error("Missing call to hestia_initialize()");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    if (auto rc = check_object_id(object_id, true); rc != 0) {
        return rc;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    std::string id;
    if (object_id->m_uuid == nullptr) {
        id = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        id = object_id->m_uuid;
    }
    auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, id.c_str(), id.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);
    if (rc != 0) {
        return rc;
    }

    if (output_size == 0) {
        set_last_error("Object not found");
        return hestia_error_e::HESTIA_ERROR_NOT_FOUND;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));

    delete[] output;
    output = nullptr;

    object->m_creation_time =
        static_cast<time_t>(get_int_map_item(attr_dict, "creation_time"));
    object->m_atime = static_cast<time_t>(
        get_int_map_item(attr_dict, "content_accessed_time"));
    auto last_modified_time = static_cast<time_t>(
        get_int_map_item(attr_dict, "content_modified_time"));
    auto metadata_modified_time = static_cast<time_t>(
        get_int_map_item(attr_dict, "metadata_modified_time"));

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
            tier_extent.m_size          = size;
            tier_extent.m_creation_time = static_cast<time_t>(
                get_int_map_item(*tier_extent_dict, "creation_time"));
            tier_extent.m_last_modified_time = static_cast<time_t>(
                get_int_map_item(*tier_extent_dict, "last_modified_time"));
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
            extents[idx].m_tier_index = tier_ids[idx];
        }

        object->m_tier_extents = new HestiaTierExtent[extents.size()];
        for (std::size_t idx = 0; idx < extents.size(); idx++) {
            object->m_tier_extents[idx] = extents[idx];
        }
        object->m_num_tier_extents = extents.size();
    }
    object->m_size  = max_size;
    object->m_mtime = last_modified_time;
    object->m_ctime = std::max(metadata_modified_time, object->m_mtime);

    if (auto name = attr_dict.get_map_item("name"); name) {
        StringUtils::to_char(name->get_scalar(), &object->m_name);
    }
    else {
        StringUtils::to_char("", &object->m_name);
    }
    StringUtils::to_char(
        attr_dict.get_map_item("id")->get_scalar(), &object->m_uuid);
    // NOLINTEND
    return 0;
}

int hestia_object_set_attrs(
    HestiaId* object_id,
    const HestiaKeyValuePair key_value_pairs[],
    size_t num_key_pairs)
{
    if (!HestiaPrivate::check_initialized()) {
        set_last_error("Missing call to hestia_initialize()");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (auto rc = check_object_id(object_id, true); rc != 0) {
        return rc;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    std::string id;
    if (object_id->m_uuid == nullptr) {
        id = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        id = object_id->m_uuid;
    }

    auto rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, id.c_str(), id.size(),
        hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output, &output_size,
        &totals);

    if (rc != 0) {
        LOG_ERROR("Error reading objects");
        return rc;
    }

    if (output_size == 0) {
        set_last_error("Object not found");
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

int hestia_object_put(
    HestiaId* object_id,
    hestia_create_mode_t create_mode,
    HestiaIoContext* io_context,
    HestiaTier* tier)
{
    if (!HestiaPrivate::check_initialized()) {
        set_last_error("Failed init check.");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    auto rc = check_object_id(object_id, false);
    if (rc != 0) {
        return rc;
    }

    rc = check_io_context(io_context);
    if (rc != 0) {
        return rc;
    }

    std::string id;
    std::tie(rc, id) = create_if_not_existing(object_id, create_mode);
    if (rc != 0) {
        return rc;
    }

    if (object_id->m_user_initialized != 2) {
        StringUtils::to_char(id, &object_id->m_uuid);
    }

    uint8_t tier_id{255};
    if (io_context->m_type != hestia_io_type_t::HESTIA_IO_EMPTY) {
        if (tier->m_user_initialized == 2) {
            tier_id = tier->m_index;
        }
        else {
            LOG_INFO("Tier not set - listing system tiers");
            uint8_t* tiers{nullptr};
            std::size_t num_tiers{0};
            if (const auto rc = hestia_list_tiers(&tiers, &num_tiers);
                rc != 0) {
                hestia_free_tier_ids(&tiers);
                return rc;
            }
            if (num_tiers == 0) {
                set_last_error(
                    "Attempted to put data but no tiers found on system.");
                {
                    return -1;
                }
            }

            for (std::size_t idx = 0; idx < num_tiers; idx++) {
                if (tiers[idx] < tier_id) {
                    tier_id = tiers[idx];
                }
            }
            hestia_free_tier_ids(&tiers);
        }
    }

    char* activity_id{nullptr};
    int len_activity_id{0};

    if (io_context->m_type == hestia_io_type_t::HESTIA_IO_BUFFER) {
        rc = hestia_data_put(
            id.c_str(), io_context->m_buffer, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_PATH) {
        rc = hestia_data_put_path(
            id.c_str(), io_context->m_path, io_context->m_length,
            io_context->m_offset, tier_id, &activity_id, &len_activity_id);
    }
    else if (io_context->m_type == hestia_io_type_t::HESTIA_IO_DESCRIPTOR) {
        rc = hestia_data_put_descriptor(
            id.c_str(), io_context->m_fd, io_context->m_length,
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

    auto rc = check_object_id(object_id, true);
    if (rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }

    char* action_id{nullptr};
    int action_id_size{0};
    rc = hestia_data_copy(
        hestia_item_t::HESTIA_OBJECT, uuid.c_str(), source_tier, target_tier,
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

    auto rc = check_object_id(object_id, true);
    if (rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }

    char* action_id{nullptr};
    int action_id_size{0};
    rc = hestia_data_move(
        hestia_item_t::HESTIA_OBJECT, uuid.c_str(), source_tier, target_tier,
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

    auto rc = check_object_id(object_id, true);
    if (rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }
    char* action_id{nullptr};
    int action_id_size{0};

    if (error_if_last_tier != 0) {
    }

    rc = hestia_data_release(
        hestia_item_t::HESTIA_OBJECT, uuid.c_str(), tier_id, &action_id,
        &action_id_size);
    delete[] action_id;
    return rc;
}

int hestia_object_remove(HestiaId* object_id)
{
    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    auto rc = check_object_id(object_id, true);
    if (rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }

    rc = hestia_remove(
        hestia_item_t::HESTIA_OBJECT, hestia_id_format_t::HESTIA_ID,
        uuid.c_str(), uuid.size());
    return rc;
}

int hestia_object_locate(
    HestiaId* object_id, uint8_t** tier_ids, size_t* num_ids)
{
    *num_ids = 0;

    if (!HestiaPrivate::check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    auto rc = check_object_id(object_id, true);
    if (rc != 0) {
        return rc;
    }

    std::string uuid;
    if (object_id->m_uuid == nullptr) {
        uuid = Uuid(object_id->m_lo, object_id->m_hi).to_string();
    }
    else {
        if (auto rc = validate_uuid(object_id); rc != 0) {
            return rc;
        }
        uuid = object_id->m_uuid;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    rc = hestia_read(
        hestia_item_t::HESTIA_OBJECT, hestia_query_format_t::HESTIA_QUERY_IDS,
        hestia_id_format_t::HESTIA_ID, 0, 0, uuid.c_str(), uuid.size(),
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
        return 0;
    }
    std::vector<uint8_t> ids;
    rc = get_tier_ids(tier_uuids, ids);
    if (rc != 0) {
        return rc;
    }

    *num_ids = ids.size();
    if (!ids.empty()) {
        *tier_ids = new uint8_t[ids.size()];
        for (std::size_t idx = 0; idx < ids.size(); idx++) {
            (*tier_ids)[idx] = ids[idx];
        }
    }
    else {
        *tier_ids = nullptr;
    }

    return 0;
}

int hestia_list_tiers(uint8_t** tier_ids, size_t* num_ids)
{
    if (!HestiaPrivate::check_initialized()) {
        set_last_error("Missing call to hestia_initialize()");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    char* output{nullptr};
    int totals{0};
    int output_size{0};

    hestia_io_format_t output_format = static_cast<hestia_io_format_t>(
        HESTIA_IO_KEY_VALUE | HESTIA_IO_NO_CHILD);
    const auto rc = hestia_read(
        hestia_item_t::HESTIA_TIER, hestia_query_format_t::HESTIA_QUERY_NONE,
        hestia_id_format_t::HESTIA_ID_NONE, 0, 0, nullptr, 0, output_format,
        &output, &output_size, &totals);
    if (rc != 0) {
        return rc;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));
    if (attr_dict.is_sequence()) {
        *num_ids          = attr_dict.get_sequence().size();
        *tier_ids         = new uint8_t[*num_ids];
        std::size_t count = 0;
        for (const auto& item : attr_dict.get_sequence()) {
            if (!item->has_map_item("priority")) {
                set_last_error(
                    "Tier is missing a 'priority' - possible config issue.");
                return -1;
            }
            const auto name = item->get_map_item("priority")->get_scalar();
            try {
                (*tier_ids)[count] = std::stoi(name);
            }
            catch (const std::exception& e) {
                set_last_error(
                    "Failed to convert tier priority to int - tier config issue.");
                return -1;
            }
            count++;
        }
    }
    else {
        *num_ids  = 1;
        *tier_ids = new uint8_t[*num_ids];
        if (!attr_dict.has_map_item("priority")) {
            set_last_error(
                "Tier is missing a 'priority' - possible config issue.");
            return -1;
        }
        const auto name = attr_dict.get_map_item("priority")->get_scalar();
        try {
            (*tier_ids)[0] = std::stoi(name);
        }
        catch (const std::exception& e) {
            set_last_error(
                "Failed to convert tier priority to int - tier config issue.");
            return -1;
        }
    }
    delete[] output;
    return 0;
}

int hestia_object_list(uint8_t tier_id, HestiaId** object_ids, size_t* num_ids)
{
    char* output{nullptr};
    int totals{0};
    int output_size{0};

    auto tier_query = "priority=" + std::to_string(tier_id);

    auto rc = hestia_read(
        hestia_item_t::HESTIA_TIER, hestia_query_format_t::HESTIA_QUERY_FILTER,
        hestia_id_format_t::HESTIA_ID, 0, 0, tier_query.c_str(),
        tier_query.size(), hestia_io_format_t::HESTIA_IO_KEY_VALUE, &output,
        &output_size, &totals);
    if (rc != 0) {
        return rc;
    }

    if (output_size == 0) {
        return hestia_error_t::HESTIA_ERROR_NOT_FOUND;
    }

    Dictionary attr_dict;
    attr_dict.from_string(std::string(output, output_size));
    delete[] output;

    auto extents = attr_dict.get_map_item("extents");
    if (extents == nullptr) {
        *num_ids = 0;
        return 0;
    }

    // Iterate object ids in extents
    std::set<std::string> object_id_strs;
    for (const auto& extent :
         attr_dict.get_map_item("extents")->get_sequence()) {
        if (extent->has_map_item("object")) {
            object_id_strs.insert(
                extent->get_map_item("object")->get_scalar("id"));
        }
    }

    std::vector<Uuid> uuids;
    for (const auto& id : object_id_strs) {
        uuids.emplace_back(Uuid::from_string(id));
    }

    *num_ids    = uuids.size();
    *object_ids = new HestiaId[uuids.size()];

    std::size_t count{0};
    for (const auto& uuid : uuids) {
        hestia_init_id(&(*object_ids)[count]);
        (*object_ids)[count].m_lo = uuid.lo();
        (*object_ids)[count].m_hi = uuid.hi();
        StringUtils::to_char(uuid.to_string(), &(*object_ids)[count].m_uuid);
        count++;
    }
    return 0;
}
}

}  // namespace hestia