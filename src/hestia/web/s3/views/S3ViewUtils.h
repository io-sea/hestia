#pragma once

#include "CrudIdentifier.h"
#include "HttpRequest.h"
#include "Map.h"
#include "S3Path.h"
#include "S3Request.h"

#include "Logger.h"

#define FAIL_CHECK(query)                                                      \
    {                                                                          \
        if (const auto status = query; !status.ok()) {                         \
            LOG_ERROR(status.m_message);                                       \
            return HttpResponse::create(500, "Internal Server Error");         \
        }                                                                      \
    }

#define CHECK_EXISTS(item)                                                     \
    {                                                                          \
        auto [status, exists] = m_service->exists(item);                       \
        if (!status.ok()) {                                                    \
            LOG_ERROR(status.m_message);                                       \
            return HttpResponse::create(500, "Internal Server Error");         \
        }                                                                      \
        if (!exists) {                                                         \
            return HttpResponse::create(404, "Not Found");                     \
        }                                                                      \
    }

#define FAIL_IF_EXISTS(item)                                                   \
    {                                                                          \
        auto [status, exists] = m_service->exists(item);                       \
        if (!status.ok()) {                                                    \
            LOG_ERROR(status.m_message);                                       \
            return HttpResponse::create(500, "Internal Server Error");         \
        }                                                                      \
        if (exists) {                                                          \
            return HttpResponse::create(409, "Conflict");                      \
        }                                                                      \
    }

namespace hestia {
class S3ViewUtils {
  public:
    static CrudIdentifier path_to_crud_id(const S3Path& path);

    static HttpResponse::Ptr on_server_error(
        const S3Request& req, const std::string& msg);

    static HttpResponse::Ptr on_no_such_bucket(const S3Request& req);

    static HttpResponse::Ptr on_no_such_key(
        const S3Request& req, bool no_bucket = false);

    static HttpResponse::Ptr on_bucket_already_exists(const S3Request& req);

    static HttpResponse::Ptr on_tried_to_delete_nonempty_bucket(
        const S3Request& req);

    static void set_common_headers(HttpResponse& response);

    static void metadata_to_header(const Map& metadata, HttpResponse* response);

    static void header_to_metadata(const HttpRequest& request, Map& metadata);
};
}  // namespace hestia