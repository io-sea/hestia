#pragma once

#include "HttpRequest.h"
#include "Map.h"
#include "S3Path.h"

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
    static void metadata_to_header(const Map& metadata, HttpResponse* response)
    {
        auto on_item =
            [&response](const std::string& key, const std::string& value) {
                response->header().set_item(S3Path::meta_prefix + key, value);
            };
        metadata.for_each_item(on_item);
    }

    static void header_to_metadata(const HttpRequest& request, Map& metadata)
    {
        metadata =
            request.get_header().get_items_with_prefix(S3Path::meta_prefix);
    }
};
}  // namespace hestia