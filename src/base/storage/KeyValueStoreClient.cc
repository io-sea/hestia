#include "KeyValueStoreClient.h"

#include "Logger.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<KeyValueStoreError>& e)                      \
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
KeyValueStoreResponse::Ptr KeyValueStoreClient::make_request(
    const KeyValueStoreRequest& request) const noexcept
{
    auto response = KeyValueStoreResponse::create(request);

    switch (request.method()) {
        case KeyValueStoreRequestMethod::STRING_EXISTS:
            try {
                const auto exists = string_exists(request.get_query().first);
                response->set_key_found(exists);
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_GET:
            try {
                string_get(request.get_query().first, response->get_value());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_SET:
            try {
                string_set(
                    request.get_query().first, request.get_query().second);
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::STRING_REMOVE:
            try {
                string_remove(request.get_query().first);
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_ADD:
            try {
                set_add(request.get_query().first, request.get_query().second);
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_LIST:
            try {
                set_list(request.get_query().first, response->get_set_items());
            }
            CATCH_FLOW();
            break;
        case KeyValueStoreRequestMethod::SET_REMOVE:
            try {
                set_remove(
                    request.get_query().first, request.get_query().second);
            }
            CATCH_FLOW();
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const KeyValueStoreError error(
                KeyValueStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
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
        const std::string msg = "Exception in " + request.method_as_string()
                                + " method: " + message;
        const KeyValueStoreError error(
            KeyValueStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
        const KeyValueStoreError error(
            KeyValueStoreErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}

void KeyValueStoreClient::on_exception(
    const KeyValueStoreRequest& request,
    KeyValueStoreResponse* response,
    const KeyValueStoreError& error) const
{
    const std::string msg =
        "Error in " + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}

KeyValueStoreClient::~KeyValueStoreClient() {}
}  // namespace hestia
