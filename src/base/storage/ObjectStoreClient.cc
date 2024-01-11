#include "ObjectStoreClient.h"

#include "ErrorUtils.h"
#include "Logger.h"
#include "RequestException.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<ObjectStoreError>& e)                        \
    {                                                                          \
        on_exception(ctx.m_request, response.get(), e.get_error());            \
        ctx.finish(std::move(response));                                       \
        return;                                                                \
    }                                                                          \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(ctx.m_request, response.get(), e.what());                 \
        ctx.finish(std::move(response));                                       \
        return;                                                                \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(ctx.m_request, response.get());                           \
        ctx.finish(std::move(response));                                       \
        return;                                                                \
    }

namespace hestia {

void ObjectStoreClient::initialize(
    const std::string& id, const std::string&, const Dictionary&)
{
    m_id = id;
}

void ObjectStoreClient::make_request(ObjectStoreContext& ctx) const noexcept
{
    auto response = ObjectStoreResponse::create(ctx.m_request, m_id);

    switch (ctx.m_request.method()) {
        case ObjectStoreRequestMethod::GET:
            try {
                get(ctx);
            }
            CATCH_FLOW();
            return;
        case ObjectStoreRequestMethod::PUT:
            try {
                put(ctx);
            }
            CATCH_FLOW();
            return;
        case ObjectStoreRequestMethod::EXISTS:
            try {
                response->set_object_found(exists(ctx.m_request.object()));
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::LIST:
            try {
                list(ctx.m_request.query(), response->objects());
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::REMOVE:
            try {
                remove(ctx.m_request.object());
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::CUSTOM:
            break;
        default:
            const std::string msg =
                "Method: " + ctx.m_request.method_as_string()
                + " not supported";
            const ObjectStoreError error(
                ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
    }
    ctx.finish(std::move(response));
}

void ObjectStoreClient::add_stream_progress_func(ObjectStoreContext& ctx) const
{
    auto stream_progress_func = [ctx, id = m_id](std::size_t transferred) {
        auto response = ObjectStoreResponse::create(ctx.m_request, id);
        response->set_bytes_transferred(transferred);
        ctx.m_progress_func(std::move(response));
    };
    ctx.m_stream->set_progress_func(
        ctx.m_request.get_progress_interval(), stream_progress_func);
}

void ObjectStoreClient::init_stream(
    ObjectStoreContext& ctx, const StorageObject& object) const
{
    if (!ctx.has_stream()) {
        return;
    }
    if (ctx.has_progress_func()) {
        add_stream_progress_func(ctx);
    }

    auto stream_completion_func = [ctx, object, id = m_id](StreamState state) {
        auto response = ObjectStoreResponse::create(ctx.m_request, id);
        if (!state.ok()) {
            response->on_error(
                {ObjectStoreErrorCode::BAD_STREAM,
                 SOURCE_LOC() + "|" + state.message()});
        }
        else {
            response->object() = object;
        }
        ctx.finish(std::move(response));
    };
    ctx.m_stream->set_completion_func(stream_completion_func);
}

void ObjectStoreClient::init_stream(ObjectStoreContext& ctx) const
{
    if (!ctx.has_stream()) {
        return;
    }
    if (ctx.has_progress_func()) {
        add_stream_progress_func(ctx);
    }

    auto stream_completion_func = [ctx, id = m_id](StreamState state) {
        auto response = ObjectStoreResponse::create(ctx.m_request, id);
        if (!state.ok()) {
            response->on_error(
                {ObjectStoreErrorCode::BAD_STREAM,
                 SOURCE_LOC() + "|" + state.message()});
        }
        ctx.finish(std::move(response));
    };
    ctx.m_stream->set_completion_func(stream_completion_func);
}

void ObjectStoreClient::on_object_not_found(
    const std::string& source_loc, const std::string& object_id) const
{
    const std::string msg =
        source_loc + " | Requested object: " + object_id + " not found.";
    LOG_ERROR(msg);
    throw ObjectStoreException({ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
}

void ObjectStoreClient::on_success(const ObjectStoreContext& ctx) const
{
    ctx.finish(ObjectStoreResponse::create(ctx.m_request, m_id));
}

void ObjectStoreClient::on_exception(
    const ObjectStoreRequest& request,
    ObjectStoreResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = SOURCE_LOC() + " | Exception in "
                                + request.method_as_string()
                                + " method: " + message;
        const ObjectStoreError error(ObjectStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
    else {
        const std::string msg = SOURCE_LOC() + " | Uknown Exception in "
                                + request.method_as_string() + " method";
        const ObjectStoreError error(
            ObjectStoreErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}

void ObjectStoreClient::on_exception(
    const ObjectStoreRequest& request,
    ObjectStoreResponse* response,
    const ObjectStoreError& error) const
{
    const std::string msg = SOURCE_LOC() + " | Error in "
                            + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}
}  // namespace hestia