#include "HestiaCApi.h"

#include "Logger.h"

#include <iostream>

namespace hestia {
HestiaType HestiaCApi::to_subject(hestia_item_t subject)
{
    switch (subject) {
        case hestia_item_e::HESTIA_OBJECT:
            return HestiaType(HsmItem::Type::OBJECT);
        case hestia_item_e::HESTIA_TIER:
            return HestiaType(HsmItem::Type::TIER);
        case hestia_item_e::HESTIA_DATASET:
            return HestiaType(HsmItem::Type::DATASET);
        case hestia_item_e::HESTIA_ACTION:
            return HestiaType(HsmItem::Type::ACTION);
        case hestia_item_e::HESTIA_USER_METADATA:
            return HestiaType(HsmItem::Type::METADATA);
        case hestia_item_e::HESTIA_TIER_EXTENT:
            return HestiaType(HsmItem::Type::EXTENT);
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

CrudIdentifier::InputFormat HestiaCApi::to_crud_id_format(
    hestia_id_format_t id_format)
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
    else if (id_format == hestia_id_format_t::HESTIA_PARENT_ID) {
        return CrudIdentifier::InputFormat::PARENT_ID;
    }
    return CrudIdentifier::InputFormat::NONE;
}

CrudAttributes::Format HestiaCApi::to_crud_attr_format(
    hestia_io_format_t io_format)
{
    if ((io_format & hestia_io_format_t::HESTIA_IO_JSON) != 0) {
        return CrudAttributes::Format::JSON;
    }
    else if ((io_format & hestia_io_format_t::HESTIA_IO_KEY_VALUE) != 0) {
        return CrudAttributes::Format::DICT;
    }
    else {
        return CrudAttributes::Format::NONE;
    }
}

bool HestiaCApi::expects_ids(
    hestia_io_format_t io_format, CrudIdentifier::InputFormat id_format)
{
    return (io_format & HESTIA_IO_IDS) != 0
           && id_format != CrudIdentifier::InputFormat::NONE;
}

bool HestiaCApi::expects_attrs(CrudMethod method, hestia_io_format_t io_format)
{
    if (method == CrudMethod::REMOVE) {
        return false;
    }
    return (io_format & HESTIA_IO_JSON) != 0
           || (io_format & HESTIA_IO_KEY_VALUE) != 0;
}

CrudQuery::BodyFormat HestiaCApi::to_output_format(hestia_io_format_t io_format)
{
    if (io_format == HESTIA_IO_IDS) {
        return CrudQuery::BodyFormat::ID;
    }
    else if (io_format == HESTIA_IO_KEY_VALUE) {
        return CrudQuery::BodyFormat::DICT;
    }
    else if (io_format == HESTIA_IO_JSON) {
        return CrudQuery::BodyFormat::JSON;
    }
    else {
        return CrudQuery::BodyFormat::NONE;
    }
}

bool HestiaCApi::has_mandatory_input(CrudMethod method)
{
    return method == CrudMethod::REMOVE;
}

int HestiaCApi::do_crud_request(
    IHestiaClient& client,
    const HestiaRequest& req,
    char** response_body,
    int* len_response)
{
    std::promise<HestiaResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb = [&response_promise](HestiaResponse::Ptr response) {
        response_promise.set_value(std::move(response));
    };

    client.make_request(req, completion_cb);

    const auto response = response_future.get();

    if (response->ok() && req.get_output_format() != CrudQuery::BodyFormat::NONE
        && response->has_content()) {
        StringUtils::to_char(response->write(), response_body, len_response);
    }
    return response->get_status().m_error_code;
}

int HestiaCApi::do_crud(
    IHestiaClient& client,
    hestia_item_t subject,
    hestia_io_format_t input_format,
    hestia_id_format_t id_format,
    const char* input,
    int len_input,
    hestia_io_format_t output_format,
    char** response_body,
    int* len_response,
    CrudMethod method)
{
    HestiaRequest req(HestiaCApi::to_subject(subject), method);
    if (input_format != HESTIA_IO_NONE) {
        if (input == nullptr) {
            return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
        }

        std::string body(input, len_input);
        std::size_t offset        = 0;
        const auto crud_id_format = HestiaCApi::to_crud_id_format(id_format);
        if (HestiaCApi::expects_ids(input_format, crud_id_format)) {
            CrudIdentifierCollection ids;

            CrudIdentifierCollection::FormatSpec format;
            format.m_id_spec.m_input_format = crud_id_format;
            offset                          = ids.load(body, format);

            req.set_ids(ids);
        }
        if (HestiaCApi::expects_attrs(method, input_format)) {

            CrudAttributes::FormatSpec format;
            format.m_type = HestiaCApi::to_crud_attr_format(input_format);
            req.append(body.substr(offset, body.size() - offset), format);
        }
    }
    else if (HestiaCApi::has_mandatory_input(method)) {
        return hestia_error_e::HESTIA_ERROR_BAD_INPUT_ID;
    }

    req.set_output_format(HestiaCApi::to_output_format(output_format));
    return do_crud_request(client, req, response_body, len_response);
}

int HestiaCApi::do_hsm_action(
    IHestiaClient& client,
    const HestiaRequest& req,
    Stream* stream,
    char** activity_id,
    int* len_activity_id,
    size_t* length)
{
    std::promise<HestiaResponse::Ptr> response_promise;
    auto response_future = response_promise.get_future();

    auto completion_cb = [&response_promise](HestiaResponse::Ptr response) {
        response_promise.set_value(std::move(response));
    };

    client.make_request(req, completion_cb, stream);

    if (stream != nullptr) {
        if (req.get_action().is_data_put_action()) {
            if (stream->waiting_for_content()) {
                auto stream_status = stream->flush();
                if (!stream_status.ok()) {
                    client.set_last_error("Bad stream");
                    return hestia_error_e::HESTIA_ERROR_BAD_STREAM;
                }
            }
            else {
                auto stream_status = stream->reset();
                if (!stream_status.ok()) {
                    client.set_last_error("Bad stream");
                    return hestia_error_e::HESTIA_ERROR_BAD_STREAM;
                }
            }
        }
        else {
            if (stream->has_source()) {
                auto stream_status = stream->flush();
                if (!stream_status.ok()) {
                    client.set_last_error("Bad stream");
                    return hestia_error_e::HESTIA_ERROR_BAD_STREAM;
                }
                if (length != nullptr) {
                    *length = stream_status.get_num_transferred();
                }
            }
            else if (length != nullptr) {
                *length = stream->get_num_transferred();
            }
        }
    }

    const auto response = response_future.get();
    if (response->ok()) {
        StringUtils::to_char(
            response->get_action_id(), activity_id, len_activity_id);
    }
    else {
        *len_activity_id = 0;
    }

    if (response->get_status().m_error_code != 0) {
        client.set_last_error(response->get_status().message());
    }
    return response->get_status().m_error_code;
}

}  // namespace hestia