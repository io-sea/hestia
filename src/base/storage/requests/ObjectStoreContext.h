#pragma once

#include "ObjectStoreRequest.h"
#include "ObjectStoreResponse.h"

#include "Stream.h"

#include <functional>

namespace hestia {
class ObjectStoreContext {
  public:
    using completionFunc = std::function<void(ObjectStoreResponse::Ptr)>;
    using progressFunc   = std::function<void(ObjectStoreResponse::Ptr)>;
    ObjectStoreContext(
        const ObjectStoreRequest& req,
        completionFunc completion_func,
        progressFunc progress_func = nullptr,
        Stream* stream             = nullptr);

    bool has_progress_func() const;

    bool has_stream() const;

    const ObjectStoreRequest m_request;
    const completionFunc m_completion_func;
    const progressFunc m_progress_func;
    Stream* m_stream{nullptr};
};
}  // namespace hestia