#pragma once

#include "CrudRequest.h"
#include "HestiaClient.h"
#include "HestiaTypes.h"
#include "hestia.h"

namespace hestia {
class HestiaCApi {
  public:
    static HestiaType to_subject(hestia_item_t subject);

    static CrudIdentifier::InputFormat to_crud_id_format(
        hestia_id_format_t id_format);

    static CrudAttributes::Format to_crud_attr_format(
        hestia_io_format_t io_format);

    static CrudQuery::BodyFormat to_output_format(hestia_io_format_t io_format);

    static bool expects_ids(
        hestia_io_format_t io_format, CrudIdentifier::InputFormat id_format);

    static bool expects_attrs(CrudMethod method, hestia_io_format_t io_format);

    static bool has_mandatory_input(CrudMethod method);

    static int do_crud_request(
        IHestiaClient& client,
        const HestiaRequest& req,
        char** response_body,
        int* len_response);

    static int do_crud(
        IHestiaClient& client,
        hestia_item_t subject,
        hestia_io_format_t input_format,
        hestia_id_format_t id_format,
        const char* input,
        int len_input,
        hestia_io_format_t output_format,
        char** response_body,
        int* len_response,
        CrudMethod method);

    static int do_hsm_action(
        IHestiaClient& client,
        const HestiaRequest& req,
        Stream* stream,
        char** activity_id,
        int* len_activity_id,
        size_t* length = nullptr);
};
}  // namespace hestia