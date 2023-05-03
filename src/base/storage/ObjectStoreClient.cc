#include "ObjectStoreClient.h"

#include "Logger.h"
#include "RequestException.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<ObjectStoreError>& e)                        \
    {                                                                          \
        on_exception(request, response.get(), e.get_error());                  \
        return response;                                                       \
    }                                                                          \
    catch (const std::exception& e)                                            \
    {                                                                          \
        on_exception(request, response.get(), e.what());                       \
        return response;                                                       \
    }                                                                          \
    catch (...)                                                                \
    {                                                                          \
        on_exception(request, response.get());                                 \
        return response;                                                       \
    }

namespace hestia {
ObjectStoreResponse::Ptr ObjectStoreClient::make_request(
    const ObjectStoreRequest& request, Stream* stream) const noexcept
{
    auto response = ObjectStoreResponse::create(request);

    switch (request.method()) {
        case ObjectStoreRequestMethod::GET:
            try {
                get(response->object(), request.extent(), stream);
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::PUT:
            try {
                put(request.object(), request.extent(), stream);
            }
            CATCH_FLOW();
            break;
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
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const ObjectStoreError error(
                ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
            return response;
    }

    return response;
}

void ObjectStoreClient::on_exception(
    const ObjectStoreRequest& request,
    ObjectStoreResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = "Exception in " + request.method_as_string()
                                + " method: " + message;
        const ObjectStoreError error(ObjectStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
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
    const std::string msg =
        "Error in " + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}
}  // namespace hestia