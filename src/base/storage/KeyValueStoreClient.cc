#include "KeyValueStoreClient.h"

#include "RequestException.h"

#include "ErrorUtils.h"
#include "Logger.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<RequestError<CrudErrorCode>>& e)             \
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

void KeyValueStoreClient::initialize(const std::string&, const Dictionary&) {}

KeyValueStoreResponse::Ptr KeyValueStoreClient::make_request(
    const KeyValueStoreRequest& request) const noexcept
{
    auto response = std::make_unique<KeyValueStoreResponse>(request);

    switch (request.method()) {
        case KeyValueStoreRequestMethod::STRING_EXISTS:
            try {
                string_exists(request.get_keys(), response->found());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_GET:
            try {
                string_get(request.get_keys(), response->items());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_SET:
            try {
                string_set(request.get_kv_pairs());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_REMOVE:
            try {
                string_remove(request.get_keys());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_ADD:
            try {
                set_add(request.get_kv_pairs());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_LIST:
            try {
                set_list(request.get_keys(), response->ids());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_REMOVE:
            try {
                set_remove(request.get_kv_pairs());
            }
            CATCH_FLOW();
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const CrudRequestError error(
                CrudErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
            return response;
    }

    return response;
}

void KeyValueStoreClient::on_exception(
    const KeyValueStoreRequest& request,
    KeyValueStoreResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = SOURCE_LOC() + " | Exception in "
                                + request.method_as_string() + " method.\n"
                                + message;
        const CrudRequestError error(CrudErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error << " msg " << message);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
        const CrudRequestError error(CrudErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}

void KeyValueStoreClient::on_exception(
    const KeyValueStoreRequest& request,
    KeyValueStoreResponse* response,
    const RequestError<CrudErrorCode>& error) const
{
    const std::string msg = SOURCE_LOC() + " | Error in "
                            + request.method_as_string() + " method. \n";
    LOG_ERROR(msg << error);
    response->on_error(error);
}

KeyValueStoreClient::~KeyValueStoreClient() {}
}  // namespace hestia
