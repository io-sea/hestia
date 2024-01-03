#include "HsmObjectStoreContext.h"

namespace hestia {
HsmObjectStoreContext::HsmObjectStoreContext(
    const HsmObjectStoreRequest& req,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) :
    m_request(req),
    m_completion_func(completion_func),
    m_progress_func(progress_func),
    m_stream(stream)
{
}

bool HsmObjectStoreContext::has_progress_func() const
{
    return m_progress_func != nullptr;
}

bool HsmObjectStoreContext::has_stream() const
{
    return m_stream != nullptr;
}

HsmObjectStoreRequest HsmObjectStoreContext::replace_method(
    const HsmObjectStoreRequest req, HsmObjectStoreRequestMethod method)
{
    HsmObjectStoreRequest output(req.object(), method);
    output.set_source_tier(req.source_tier());
    output.set_target_tier(req.target_tier());
    output.set_extent(req.extent());
    return output;
}

HsmObjectStoreContext HsmObjectStoreContext::replace_method(
    HsmObjectStoreRequestMethod method) const
{
    return {
        replace_method(m_request, method), m_completion_func, m_progress_func,
        m_stream};
}

ObjectStoreContext HsmObjectStoreContext::to_object_store_context() const
{
    auto completion_cb = [ctx = *this](ObjectStoreResponse::Ptr response) {
        ctx.m_completion_func(
            HsmObjectStoreResponse::create(ctx.m_request, std::move(response)));
    };
    auto progress_cb = [ctx = *this](ObjectStoreResponse::Ptr response) {
        ctx.m_progress_func(
            HsmObjectStoreResponse::create(ctx.m_request, std::move(response)));
    };
    return {
        HsmObjectStoreRequest::to_base_request(m_request), completion_cb,
        progress_cb, m_stream};
}
}  // namespace hestia