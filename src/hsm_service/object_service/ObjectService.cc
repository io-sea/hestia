#include "ObjectService.h"

#include "HsmObjectAdapter.h"
#include "JsonUtils.h"
#include "KeyValueStoreClient.h"
#include "Logger.h"

#define CATCH_FLOW()                                                           \
    catch (const RequestException<ObjectServiceError>& e)                      \
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
ObjectService::ObjectService(
    KeyValueStoreClient* client,
    HttpClient* http_client,
    std::unique_ptr<HsmObjectAdapter> object_adatper) :
    m_kv_store_client(client),
    m_http_client(http_client),
    m_object_adapter(
        object_adatper ? std::move(object_adatper) : HsmObjectAdapter::create())
{
    (void)m_http_client;
}

ObjectService::~ObjectService() {}

ObjectServiceResponse::Ptr ObjectService::make_request(
    const ObjectServiceRequest& request) const noexcept
{
    auto response = ObjectServiceResponse::create(request);

    switch (request.method()) {
        case ObjectServiceRequestMethod::GET:
            try {
                get(response->object());
            }
            CATCH_FLOW();
            break;
        case ObjectServiceRequestMethod::PUT:
            try {
                put(request.object());
            }
            CATCH_FLOW();
            break;
        case ObjectServiceRequestMethod::EXISTS:
            try {
                response->set_object_found(exists(request.object()));
            }
            CATCH_FLOW();
            break;
        case ObjectServiceRequestMethod::LIST:
            try {
                list(response->objects());
            }
            CATCH_FLOW();
            break;
        case ObjectServiceRequestMethod::REMOVE:
            try {
                remove(request.object());
            }
            CATCH_FLOW();
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const ObjectServiceError error(
                ObjectServiceErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
            return response;
    }

    return response;
}

void ObjectService::get(HsmObject& object) const
{
    const std::string key = "hestia:object:" + object.object().id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_GET, query);
    const auto response = m_kv_store_client->make_request(request);

    JsonUtils::from_json(response->get_value(), object.object().m_metadata);
}

void ObjectService::put(const HsmObject& object) const
{
    const std::string key = "hestia:object:" + object.object().id();
    const Metadata::Query query{
        key, JsonUtils::to_json(object.object().m_metadata)};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_SET, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_SET: "
            + response->get_error().to_string());
    }

    const std::string set_key = "hestia:objects";
    const Metadata::Query set_add_query{set_key, object.object().id()};
    KeyValueStoreRequest set_request(
        KeyValueStoreRequestMethod::SET_ADD, set_add_query);
    const auto set_response = m_kv_store_client->make_request(set_request);
    if (!set_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_ADD: " + response->get_error().to_string());
    }
}

bool ObjectService::exists(const HsmObject& object) const
{
    const std::string key = "hestia:object:" + object.object().id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(
        KeyValueStoreRequestMethod::STRING_EXISTS, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_EXISTS: "
            + response->get_error().to_string());
    }
    return response->has_key();
}

void ObjectService::remove(const HsmObject& object) const
{
    const std::string key = "hestia:object:" + object.object().id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest string_request(
        KeyValueStoreRequestMethod::STRING_REMOVE, query);
    const auto string_response =
        m_kv_store_client->make_request(string_request);
    if (!string_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_REMOVE: "
            + string_response->get_error().to_string());
    }

    const std::string set_key = "hestia:objects";
    const Metadata::Query set_remove_query{set_key, object.object().id()};
    KeyValueStoreRequest set_request(
        KeyValueStoreRequestMethod::SET_REMOVE, set_remove_query);
    const auto set_response = m_kv_store_client->make_request(set_request);
    if (!set_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_REMOVE: "
            + set_response->get_error().to_string());
    }
}

void ObjectService::list(std::vector<HsmObject>& objects) const
{
    const std::string key = "hestia:objects";
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::SET_LIST, query);
    const auto response = m_kv_store_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_LIST: " + response->get_error().to_string());
    }

    for (const auto& item : response->get_set_items()) {
        objects.push_back(hestia::StorageObject(item));
    }
}

void ObjectService::on_exception(
    const ObjectServiceRequest& request,
    ObjectServiceResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = "Exception in " + request.method_as_string()
                                + " method: " + message;
        const ObjectServiceError error(
            ObjectServiceErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error << " " << msg);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
        const ObjectServiceError error(
            ObjectServiceErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}

void ObjectService::on_exception(
    const ObjectServiceRequest& request,
    ObjectServiceResponse* response,
    const ObjectServiceError& error) const
{
    const std::string msg =
        "Error in " + request.method_as_string() + " method: ";
    LOG_ERROR(msg << error);
    response->on_error(error);
}


}  // namespace hestia
