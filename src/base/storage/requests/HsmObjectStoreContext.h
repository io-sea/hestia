#pragma once

#include "HsmObjectStoreRequest.h"
#include "HsmObjectStoreResponse.h"
#include "ObjectStoreContext.h"

#include "Stream.h"

#include <functional>

namespace hestia {
class HsmObjectStoreContext {
  public:
    using completionFunc = std::function<void(HsmObjectStoreResponse::Ptr)>;
    using progressFunc   = std::function<void(HsmObjectStoreResponse::Ptr)>;
    HsmObjectStoreContext(
        const HsmObjectStoreRequest& req,
        completionFunc completion_func,
        progressFunc progress_func,
        Stream* stream);

    bool has_progress_func() const;

    bool has_stream() const;

    ObjectStoreContext to_object_store_context() const;

    static HsmObjectStoreRequest replace_method(
        const HsmObjectStoreRequest req, HsmObjectStoreRequestMethod method);

    HsmObjectStoreContext replace_method(
        HsmObjectStoreRequestMethod method) const;

    const HsmObjectStoreRequest m_request;
    const completionFunc m_completion_func;
    const progressFunc m_progress_func;
    Stream* m_stream{nullptr};
};
}  // namespace hestia