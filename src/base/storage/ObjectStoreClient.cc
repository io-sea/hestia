#include "ObjectStoreClient.h"

#include "ErrorUtils.h"
#include "Logger.h"
#include "RequestException.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<ObjectStoreError>& e)                        \
    {                                                                          \
        on_exception(request, response.get(), e.get_error());                  \
        completion_func(std::move(response));                                  \
        return;                                                                \
    }                                                                          \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(request, response.get(), e.what());                       \
        completion_func(std::move(response));                                  \
        return;                                                                \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(request, response.get());                                 \
        completion_func(std::move(response));                                  \
        return;                                                                \
    }

namespace hestia {
void ObjectStoreClient::make_request(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    progressFunc progress_func,
    Stream* stream) const noexcept
{
    auto response = ObjectStoreResponse::create(request, m_id);

    switch (request.method()) {
        case ObjectStoreRequestMethod::GET:
            try {
                if (progress_func == nullptr) {
                    get(request, completion_func, stream, nullptr);
                }
                else {
                    auto stream_progress_func =
                        [progress_func, request,
                         id = m_id](std::size_t transferred) {
                            auto response =
                                ObjectStoreResponse::create(request, id);
                            response->set_bytes_transferred(transferred);
                            progress_func(std::move(response));
                        };
                    get(request, completion_func, stream, stream_progress_func);
                }
            }
            CATCH_FLOW();
            return;
        case ObjectStoreRequestMethod::PUT:
            try {
                if (progress_func == nullptr) {
                    put(request, completion_func, stream, nullptr);
                }
                else {
                    auto stream_progress_func =
                        [progress_func, request,
                         id = m_id](std::size_t transferred) {
                            auto response =
                                ObjectStoreResponse::create(request, id);
                            response->set_bytes_transferred(transferred);
                            progress_func(std::move(response));
                        };
                    put(request, completion_func, stream, stream_progress_func);
                }
            }
            CATCH_FLOW();
            return;
        case ObjectStoreRequestMethod::EXISTS:
            try {
                response->set_object_found(exists(request.object()));
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::LIST:
            try {
                list(request.query(), response->objects());
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::REMOVE:
            try {
                remove(request.object());
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::CUSTOM:
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const ObjectStoreError error(
                ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
    }
    completion_func(std::move(response));
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