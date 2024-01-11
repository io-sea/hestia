#include "ObjectStoreContext.h"

namespace hestia {
ObjectStoreContext::ObjectStoreContext(
    const ObjectStoreRequest& req,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) :
    m_request(req),
    m_progress_func(progress_func),
    m_stream(stream),
    m_completion_func(completion_func)
{
}

bool ObjectStoreContext::has_progress_func() const
{
    return m_progress_func != nullptr;
}

bool ObjectStoreContext::has_stream() const
{
    return m_stream != nullptr;
}

ObjectStoreContext::completionFunc ObjectStoreContext::get_completion_func()
    const
{
    return m_completion_func;
}

void ObjectStoreContext::finish(ObjectStoreResponse::Ptr response) const
{
    m_completion_func(std::move(response));
}

}  // namespace hestia