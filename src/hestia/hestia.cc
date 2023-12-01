#include "hestia.h"

#include "hestia_private.h"

#include "HestiaCApi.h"
#include "HestiaClient.h"
#include "HestiaServer.h"

#include "HsmItem.h"
#include "JsonDocument.h"
#include "JsonUtils.h"
#include "Logger.h"
#include "Stream.h"
#include "StringUtils.h"
#include "UuidUtils.h"

#include "FileStreamSink.h"
#include "FileStreamSource.h"
#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include <cstring>
#include <future>
#include <iostream>
#include <thread>

namespace hestia {

static std::unique_ptr<IHestiaClient> g_client;
static std::unique_ptr<HestiaServer> g_server;

void HestiaPrivate::override_client(std::unique_ptr<IHestiaClient> client)
{
    g_client = std::move(client);
}

IHestiaClient* HestiaPrivate::get_client()
{
    return g_client.get();
}

bool HestiaPrivate::check_initialized()
{
    if (!g_client) {
        std::cerr
            << "Hestia client not initialized. Call 'hestia_initilaize()' first.";
        return false;
    }
    return true;
}

#define ID_CHECK(oid)                                                          \
    if (oid == nullptr) {                                                      \
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;                      \
    }

#define STATE_CHECK                                                            \
    if (!HestiaPrivate::check_initialized()) {                                 \
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;                      \
    }

extern "C" {

int hestia_start_server(const char* host, int port, const char* config)
{
    if (g_server) {
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    const std::string config_str =
        config == nullptr ? std::string() : std::string(config);

    const std::string host_str = host == nullptr ? std::string() : host;

    Dictionary config_dict;
    if (!config_str.empty()) {
        try {
            JsonDocument(config_str).write(config_dict);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to parse extra_config json in server init(): "
                      << e.what() << "\n"
                      << config_str << std::endl;
            return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
        }
    }

    try {
        g_server = std::make_unique<HestiaServer>();
        g_server->initialize("", "", config_dict, host_str, port);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to initialize Hestia Server: " << e.what()
                  << std::endl;
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    OpStatus status;
    try {
        status = g_server->run();
    }
    catch (const std::exception& e) {
        status = OpStatus(OpStatus::Status::ERROR, -1, e.what());
    }
    catch (...) {
        status = OpStatus(
            OpStatus::Status::ERROR, -1, "Unknown exception running server.");
    }
    return status.ok() ? 0 : -1;
}

int hestia_stop_server()
{
    if (!g_server) {
        std::cerr
            << "Called finish() but no server is set. Possibly missing initialize() call.";
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }

    try {
        g_server.reset();
    }
    catch (const std::exception& e) {
        std::cerr << "Error destorying Hestia server: " << e.what()
                  << std::endl;
        return hestia_error_e::HESTIA_ERROR_CLIENT_STATE;
    }
    return 0;
}

void hestia_output_info()
{
    std::cout << g_client->get_runtime_info() << std::endl;
}

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
        extra_config == nullptr ? std::string() : std::string(extra_config);

    Dictionary extra_config_dict;
    if (!extra_config_str.empty()) {
        try {
            JsonDocument(extra_config_str).write(extra_config_dict);
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to parse extra_config json in client init(): "
                      << e.what() << "\n"
                      << extra_config_str << std::endl;
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

int hestia_read(
    hestia_item_t subject,
    hestia_query_format_t query_format,
    hestia_id_format_t id_format,
    int offset,
    int count,
    const char* query,
    int len_query,
    hestia_io_format_t output_format,
    char** response_body,
    int* len_response,
    int*)
{
    STATE_CHECK

    HestiaRequest req(HestiaCApi::to_subject(subject), CrudMethod::READ);

    std::string body;
    if (query_format != HESTIA_QUERY_NONE) {
        if (query == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
        }
        body = std::string(query, len_query);
    }

    if (query_format == HESTIA_QUERY_IDS) {
        CrudIdentifierCollection::FormatSpec format;
        format.m_id_spec.m_input_format =
            HestiaCApi::to_crud_id_format(id_format);
        body = std::string(query, len_query);
        req.set_ids({body, format});
    }
    else if (query_format == HESTIA_QUERY_FILTER) {
        Map::FormatSpec format;
        req.set_query_filter({body, format});
    }

    req.set_offset_and_count(offset, count);
    req.set_output_format(HestiaCApi::to_output_format(output_format));
    return HestiaCApi::do_crud_request(
        *g_client, req, response_body, len_response);
}

int hestia_create(
    hestia_item_t subject,
    hestia_io_format_t input_format,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** response,
    int* len_response)
{
    STATE_CHECK
    return HestiaCApi::do_crud(
        *g_client, subject, input_format, id_format, input, len_input,
        output_format, response, len_response, CrudMethod::CREATE);
}

int hestia_update(
    hestia_item_t subject,
    hestia_io_format_t input_format,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** response,
    int* len_response)
{
    STATE_CHECK
    return HestiaCApi::do_crud(
        *g_client, subject, input_format, id_format, input, len_input,
        output_format, response, len_response, CrudMethod::UPDATE);
}

int hestia_remove(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input)
{
    STATE_CHECK
    return HestiaCApi::do_crud(
        *g_client, subject, hestia_io_format_t::HESTIA_IO_IDS, id_format, input,
        len_input, hestia_io_format_t::HESTIA_IO_NONE, nullptr, 0,
        CrudMethod::REMOVE);
}

int hestia_free_output(char** output)
{
    delete[] (*output);
    return 0;
}

int hestia_identify(
    hestia_item_t subject,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    const char* output,
    int* len_output,
    int* exists)
{
    (void)subject;
    (void)id_format;
    (void)input;
    (void)len_input;
    (void)output;
    (void)len_output;
    (void)exists;
    return -1;
}

int hestia_data_put(
    const char* oid,
    const void* buf,
    const size_t length,
    const size_t offset,
    const uint8_t target_tier,
    char** activity_id,
    int* len_activity_id)
{
    if (buf == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    STATE_CHECK;
    ID_CHECK(oid);

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_offset(offset);
    action.set_target_tier(target_tier);
    action.set_subject_key(std::string(oid));

    HestiaRequest req(HsmItem::Type::OBJECT, action);

    hestia::Stream stream;
    stream.set_source(hestia::InMemoryStreamSource::create(
        hestia::ReadableBufferView(buf, length)));

    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id);
}

int hestia_data_put_path(
    const char* oid,
    const char* path,
    const size_t length,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id)
{
    if (path == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    STATE_CHECK;
    ID_CHECK(oid);

    hestia::Stream stream;
    stream.set_source(FileStreamSource::create(path));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_offset(offset);
    action.set_target_tier(tier);
    action.set_subject_key(std::string(oid));

    if (length == 0) {
        action.set_size(stream.get_source_size());
    }
    else {
        action.set_size(length);
    }

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id);
}

int hestia_data_put_descriptor(
    const char* oid,
    int fd,
    const size_t length,
    const size_t offset,
    const uint8_t target_tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(oid);

    hestia::Stream stream;
    stream.set_source(FileStreamSource::create(fd, length));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::PUT_DATA);
    action.set_offset(offset);
    action.set_target_tier(target_tier);
    action.set_subject_key(oid);
    action.set_size(length);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id);
}

int hestia_data_get(
    const char* oid,
    void* buf,
    size_t* length,
    const size_t offset,
    const uint8_t src_tier,
    char** activity_id,
    int* len_activity_id)
{
    if (buf == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    if (length == nullptr) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_BUFFER;
    }

    STATE_CHECK;
    ID_CHECK(oid);

    hestia::WriteableBufferView writeable_buffer(buf, *length);
    hestia::Stream stream;
    stream.set_sink(hestia::InMemoryStreamSink::create(writeable_buffer));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_offset(offset);
    action.set_source_tier(src_tier);
    action.set_subject_key(oid);
    action.set_size(*length);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id, length);
}

int hestia_data_get_descriptor(
    const char* oid,
    int file_discriptor,
    const size_t length,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(oid);

    hestia::Stream stream;
    stream.set_sink(FileStreamSink::create(file_discriptor, length));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_offset(offset);
    action.set_source_tier(tier);
    action.set_subject_key(oid);
    action.set_size(length);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id);
}

int hestia_data_get_path(
    const char* oid,
    const char* path,
    const size_t len,
    const size_t offset,
    const uint8_t tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(oid);

    hestia::Stream stream;
    stream.set_sink(FileStreamSink::create(path));

    HsmAction action(HsmItem::Type::OBJECT, HsmAction::Action::GET_DATA);
    action.set_offset(offset);
    action.set_source_tier(tier);
    action.set_subject_key(oid);

    if (len > 0) {
        action.set_size(len);
    }

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, &stream, activity_id, len_activity_id);
}

int hestia_data_copy(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    const uint8_t tgt_tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(id);

    HsmAction action(
        HestiaCApi::to_subject(subject).m_hsm_type,
        HsmAction::Action::COPY_DATA);
    action.set_source_tier(src_tier);
    action.set_target_tier(tgt_tier);
    action.set_subject_key(id);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, nullptr, activity_id, len_activity_id);
}

int hestia_data_move(
    hestia_item_t subject,
    const char* id,
    const std::uint8_t src_tier,
    const std::uint8_t tgt_tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(id);

    HsmAction action(
        HestiaCApi::to_subject(subject).m_hsm_type,
        HsmAction::Action::MOVE_DATA);
    action.set_source_tier(src_tier);
    action.set_target_tier(tgt_tier);
    action.set_subject_key(id);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, nullptr, activity_id, len_activity_id);
}

int hestia_data_release(
    hestia_item_t subject,
    const char* id,
    const uint8_t src_tier,
    char** activity_id,
    int* len_activity_id)
{
    STATE_CHECK;
    ID_CHECK(id);

    HsmAction action(
        HestiaCApi::to_subject(subject).m_hsm_type,
        HsmAction::Action::RELEASE_DATA);
    action.set_source_tier(src_tier);
    action.set_subject_key(id);

    HestiaRequest req(HsmItem::Type::OBJECT, action);
    return HestiaCApi::do_hsm_action(
        *g_client, req, nullptr, activity_id, len_activity_id);
}

void hestia_get_last_error(char* message_buffer, size_t len_message_buffer)
{
    std::string msg;
    g_client->get_last_error(msg);
    msg += "\n**************\nHestia config:\n";
    msg += g_client->get_runtime_info();
    std::strncpy(message_buffer, msg.c_str(), len_message_buffer);
}
}
}  // namespace hestia