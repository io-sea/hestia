#include "hestia.h"

#include "HestiaClient.h"
#include "HsmItem.h"
#include "JsonUtils.h"
#include "Logger.h"
#include "Stream.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include <iostream>
#include <thread>

namespace hestia {

static std::unique_ptr<HestiaClient> g_client;

#define ID_AND_STATE_CHECK(oid)                                                \
    if (oid == nullptr) {                                                      \
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;                      \
    }                                                                          \
    if (!check_initialized()) {                                                \
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;                      \
    }

HestiaType to_subject(hestia_item_t subject)
{
    switch (subject) {
        case hestia_item_e::HESTIA_OBJECT:
            return HestiaType(HsmItem::Type::OBJECT);
        case hestia_item_e::HESTIA_TIER:
            return HestiaType(HsmItem::Type::TIER);
        case hestia_item_e::HESTIA_DATASET:
            return HestiaType(HsmItem::Type::DATASET);
        case hestia_item_e::HESTIA_USER:
            return HestiaType(HestiaType::SystemType::USER);
        case hestia_item_e::HESTIA_NODE:
            return HestiaType(HestiaType::SystemType::HSM_NODE);
        case hestia_item_e::HESTIA_ITEM_TYPE_COUNT:
            return HsmItem::Type::UNKNOWN;
        default:
            return HsmItem::Type::UNKNOWN;
    }
}

CrudIdentifier::InputFormat to_crud_id_format(hestia_id_format_t id_format)
{
    if (id_format == hestia_id_format_t::HESTIA_ID_NONE) {
        return CrudIdentifier::InputFormat::NONE;
    }
    else if (id_format == hestia_id_format_t::HESTIA_ID) {
        return CrudIdentifier::InputFormat::ID;
    }
    else if (id_format == hestia_id_format_t::HESTIA_NAME) {
        return CrudIdentifier::InputFormat::NAME;
    }
    else if (
        id_format
        == (hestia_id_format_t::HESTIA_ID
            | hestia_id_format_t::HESTIA_PARENT_ID)) {
        return CrudIdentifier::InputFormat::ID_PARENT_ID;
    }
    else if (
        id_format
        == (hestia_id_format_t::HESTIA_ID
            | hestia_id_format_t::HESTIA_PARENT_NAME)) {
        return CrudIdentifier::InputFormat::ID_PARENT_NAME;
    }
    else if (
        id_format
        == (hestia_id_format_t::HESTIA_NAME
            | hestia_id_format_t::HESTIA_PARENT_NAME)) {
        return CrudIdentifier::InputFormat::NAME_PARENT_NAME;
    }
    else if (
        id_format
        == (hestia_id_format_t::HESTIA_NAME
            | hestia_id_format_t::HESTIA_PARENT_ID)) {
        return CrudIdentifier::InputFormat::NAME_PARENT_ID;
    }
    return CrudIdentifier::InputFormat::NONE;
}

CrudAttributes::Format to_crud_attr_format(
    hestia_attribute_format_t attr_format)
{
    switch (attr_format) {
        case hestia_attribute_format_t::HESTIA_ATTRS_NONE:
            return CrudAttributes::Format::NONE;
        case hestia_attribute_format_t::HESTIA_ATTRS_JSON:
            return CrudAttributes::Format::JSON;
        case hestia_attribute_format_t::HESTIA_ATTRS_KEY_VALUE:
            return CrudAttributes::Format::KV_PAIR;
        case hestia_attribute_format_t::HESTIA_ATTRS_TYPE_COUNT:
        default:
            return CrudAttributes::Format::NONE;
    }
}

extern "C" {
int hestia_initialize(
    const char* config_path, const char* token, const char* extra_config)
{
    if (g_client) {
        g_client->set_last_error(
            "Attempted to initialize client while already running. Call 'finish()' first. ");
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    const std::string config_path_str =
        config_path == nullptr ? std::string() : config_path;
    const std::string token_str = token == nullptr ? std::string() : token;
    const std::string extra_config_str =
        extra_config == nullptr ? std::string() : extra_config;

    Dictionary extra_config_dict;
    if (!extra_config_str.empty()) {
        try {
            JsonUtils::from_json(extra_config_str, extra_config_dict);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to parse extra_config json in client init(): "
                      << e.what() << std::endl;
            return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
        }
    }

    try {
        g_client = std::make_unique<HestiaClient>();
        g_client->initialize(config_path_str, token_str, extra_config_dict);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize Hestia Client: " << e.what()
                  << std::endl;
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    return 0;
}

int hestia_finish()
{
    if (!g_client) {
        std::cerr
            << "Called finish() but no client is set. Possibly missing initialize() call.";
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    try {
        g_client.reset();
    }
    catch (const std::exception& e) {
        std::cerr << "Error destorying Hestia client: " << e.what()
                  << std::endl;
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    return 0;
}

bool check_initialized()
{
    if (!g_client) {
        std::cerr
            << "Hestia client not initialized. Call 'hestia_initilaize()' first.";
        return false;
    }
    return true;
}

void str_to_char(const std::string& str, char** chars)
{
    *chars = new char[str.size() + 1];
    for (std::size_t idx = 0; idx < str.size(); idx++) {
        (*chars)[idx] = str[idx];
    }
    (*chars)[str.size()] = '\0';
}

int process_results(
    hestia_output_format_t output_format,
    const VecCrudIdentifier& ids,
    const CrudAttributes& attrs,
    char** response,
    int* len_response)
{
    if (output_format == HESTIA_OUTPUT_NONE) {
        len_response = 0;
        return 0;
    }

    std::string response_body;
    if ((output_format & HESTIA_OUTPUT_IDS) != 0) {
        std::size_t count{0};
        for (const auto& id : ids) {
            response_body += id.get_primary_key();
            if (count < ids.size() - 1) {
                response_body += '\n';
            }
            count++;
        }
    }

    if ((output_format & HESTIA_OUTPUT_ATTRS_JSON) != 0
        || (output_format & HESTIA_OUTPUT_ATTRS_KEY_VALUE) != 0) {
        if (!response_body.empty()) {
            response_body += '\n';
        }
        response_body += attrs.get_buffer();
    }

    if (!response_body.empty()) {
        str_to_char(response_body, response);
        *len_response = response_body.size() + 1;
    }
    else {
        *len_response = 0;
    }
    return 0;
}

int create_or_update(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    hestia_attribute_format_t attribute_format,
    hestia_output_format_t output_format,
    const char* input,
    int len_input,
    char** response,
    int* len_response,
    bool is_create)
{
    if (!check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    std::vector<CrudIdentifier> ids;
    CrudAttributes attributes;

    const auto crud_id_format   = to_crud_id_format(id_format);
    const auto crud_attr_format = to_crud_attr_format(attribute_format);
    const auto crud_output_format =
        output_format == HESTIA_OUTPUT_ATTRS_KEY_VALUE ?
            CrudAttributes::Format::KV_PAIR :
            CrudAttributes::Format::JSON;

    if (crud_id_format != CrudIdentifier::InputFormat::NONE
        || crud_attr_format != CrudAttributes::Format::NONE) {
        if (input == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
        }
        std::string body(input, len_input);

        const auto offset = CrudIdentifier::parse(body, crud_id_format, ids);
        if (crud_attr_format != CrudAttributes::Format::NONE) {
            attributes.set_buffer(
                body.substr(offset, body.size() - offset), crud_attr_format);
        }
    }

    if (is_create) {
        if (const auto status = g_client->create(
                to_subject(subject), ids, attributes, crud_output_format);
            !status.ok()) {
            return status.m_error_code;
        }
    }
    else {
        if (const auto status = g_client->update(
                to_subject(subject), ids, attributes, crud_output_format);
            !status.ok()) {
            return status.m_error_code;
        }
    }

    return process_results(
        output_format, ids, attributes, response, len_response);
}

int hestia_create(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    hestia_attribute_format_t attribute_format,
    hestia_output_format_t output_format,
    const char* input,
    int len_input,
    char** response,
    int* len_response)
{
    return create_or_update(
        subject, id_format, attribute_format, output_format, input, len_input,
        response, len_response, true);
}

int hestia_update(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    hestia_attribute_format_t attribute_format,
    hestia_output_format_t output_format,
    const char* input,
    int len_input,
    char** response,
    int* len_response)
{
    return create_or_update(
        subject, id_format, attribute_format, output_format, input, len_input,
        response, len_response, false);
}

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
    int* total_count)
{
    if (!check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    (void)total_count;

    std::string query_body;
    if (query_format != HESTIA_QUERY_NONE) {
        if (query == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
        }
        query_body = std::string(query, len_query);
    }

    CrudQuery crud_query;
    if (query_format == HESTIA_QUERY_IDS) {
        std::vector<CrudIdentifier> ids;
        const auto crud_id_format = to_crud_id_format(id_format);
        CrudIdentifier::parse(query_body, crud_id_format, ids);
        crud_query.set_ids(ids);
    }
    else if (query_format == HESTIA_QUERY_INDEX) {
        auto [key, value] = StringUtils::split_on_first(query_body, ',');
        StringUtils::trim(key);
        StringUtils::trim(value);
        crud_query.set_index({key, value});
    }

    crud_query.set_offset(offset);
    crud_query.set_count(count);

    const auto crud_attrs_output_format =
        (output_format & HESTIA_OUTPUT_ATTRS_KEY_VALUE) != 0 ?
            CrudAttributes::Format::KV_PAIR :
            CrudAttributes::Format::JSON;
    crud_query.set_attributes_output_format(crud_attrs_output_format);

    CrudQuery::OutputFormat crud_output_format{CrudQuery::OutputFormat::NONE};
    if (output_format == HESTIA_OUTPUT_IDS) {
        crud_output_format = CrudQuery::OutputFormat::ID;
    }
    else if (
        output_format == HESTIA_OUTPUT_ATTRS_KEY_VALUE
        || output_format == HESTIA_OUTPUT_ATTRS_JSON) {
        crud_output_format = CrudQuery::OutputFormat::ATTRIBUTES;
    }
    else {
        crud_output_format = CrudQuery::OutputFormat::ATTRIBUTES_AND_ID;
    }
    crud_query.set_output_format(crud_output_format);

    const auto status = g_client->read(to_subject(subject), crud_query);
    if (!status.ok()) {
        return status.m_error_code;
    }

    return process_results(
        output_format, crud_query.ids(), crud_query.get_attributes(), response,
        len_response);
}

int hestia_remove(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input)
{
    if (!check_initialized()) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    if (input == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    std::vector<CrudIdentifier> ids;
    const auto crud_id_format = to_crud_id_format(id_format);

    std::string body(input, len_input);
    CrudIdentifier::parse(body, crud_id_format, ids);

    const auto status = g_client->remove(to_subject(subject), ids);
    return status.m_error_code;
}

int hestia_data_put(
    const char* oid,
    const void* buf,
    const size_t length,
    const size_t offset,
    const uint8_t target_tier)
{
    if (buf == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    ID_AND_STATE_CHECK(oid);

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(buf, length)));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_offset(offset);
    action.set_target_tier(target_tier);

    OpStatus status;
    auto completion_cb =
        [&status](OpStatus ret_status, const HsmAction& action) {
            status = ret_status;
            (void)action;
        };

    g_client->do_data_io_action(action, &stream, completion_cb);
    (void)stream.flush();

    return status.m_error_code;
}

int hestia_data_put_descriptor(
    const char* oid,
    int fd,
    const size_t length,
    const size_t offset,
    const uint8_t target_tier)
{
    ID_AND_STATE_CHECK(oid);

    (void)fd;

    hestia::Stream stream;
    // stream.set_source(hestia::::create(
    // hestia::ReadableBufferView(buf, length)));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_offset(offset);
    action.set_target_tier(target_tier);
    action.set_subject_key(oid);
    action.set_size(length);
    action.set_offset(offset);

    OpStatus status;
    auto completion_cb =
        [&status](OpStatus ret_status, const HsmAction& action) {
            status = ret_status;
            (void)action;
        };

    g_client->do_data_io_action(action, &stream, completion_cb);
    (void)stream.flush();

    return status.m_error_code;
}

int hestia_data_get(
    const char* oid,
    void* buf,
    const size_t length,
    const size_t offset,
    const uint8_t src_tier)
{
    if (buf == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    ID_AND_STATE_CHECK(oid);

    hestia::WriteableBufferView writeable_buffer(buf, length);
    hestia::Stream stream;
    stream.set_sink(hestia::InMemoryStreamSink::create(writeable_buffer));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_offset(offset);
    action.set_source_tier(src_tier);
    action.set_subject_key(oid);
    action.set_size(length);
    action.set_offset(offset);

    OpStatus status;
    auto completion_cb =
        [&status](OpStatus ret_status, const HsmAction& action) {
            status = ret_status;
            (void)action;
        };

    g_client->do_data_io_action(action, &stream, completion_cb);
    (void)stream.flush();

    return status.m_error_code;
}

int hestia_data_copy(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier)
{
    ID_AND_STATE_CHECK(id);

    HsmAction action(
        to_subject(subject).m_hsm_type, HsmAction::Action::COPY_DATA);
    action.set_source_tier(src_tier);
    action.set_target_tier(tgt_tier);
    action.set_subject_key(id);

    const auto status = g_client->do_data_movement_action(action);
    return status.m_error_code;
}

int hestia_data_move(
    hestia_item_t subject,
    const char* id,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier)
{
    ID_AND_STATE_CHECK(id);

    HsmAction action(
        to_subject(subject).m_hsm_type, HsmAction::Action::MOVE_DATA);
    action.set_source_tier(src_tier);
    action.set_target_tier(tgt_tier);
    action.set_subject_key(id);

    const auto status = g_client->do_data_movement_action(action);
    return status.m_error_code;
}

int hestia_data_release(
    hestia_item_t subject, const char* id, const uint8_t src_tier)
{
    ID_AND_STATE_CHECK(id);

    HsmAction action(
        to_subject(subject).m_hsm_type, HsmAction::Action::RELEASE_DATA);
    action.set_source_tier(src_tier);
    action.set_subject_key(id);

    const auto status = g_client->do_data_movement_action(action);
    return status.m_error_code;
}
}
}  // namespace hestia