#include "HsmKeyValueStore.h"

#include "JsonUtils.h"
#include "KeyValueStoreClient.h"
#include "Logger.h"

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
HsmKeyValueStore::HsmKeyValueStore(
    std::unique_ptr<KeyValueStoreClient> client) :
    m_client(std::move(client))
{
}

HsmKeyValueStore::~HsmKeyValueStore() {}

ObjectStoreResponse::Ptr HsmKeyValueStore::make_request(
    const ObjectStoreRequest& request) const noexcept
{
    auto response = ObjectStoreResponse::create(request);

    switch (request.method()) {
        case ObjectStoreRequestMethod::GET:
            try {
                get(response->object());
            }
            CATCH_FLOW();
            break;
        case ObjectStoreRequestMethod::PUT:
            try {
                put(request.object());
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
                list_objects(response->objects());
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
            return response;
    }

    return response;
}

void HsmKeyValueStore::get(StorageObject& object) const
{
    const std::string key = "hestia:object:" + object.id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_GET, query);
    const auto response = m_client->make_request(request);

    JsonUtils::from_json(response->get_value(), object.m_metadata);
}

void HsmKeyValueStore::put(const StorageObject& object) const
{
    const std::string key = "hestia:object:" + object.id();
    const Metadata::Query query{key, JsonUtils::to_json(object.m_metadata)};

    KeyValueStoreRequest request(KeyValueStoreRequestMethod::STRING_SET, query);
    const auto response = m_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_SET: "
            + response->get_error().to_string());
    }
}

bool HsmKeyValueStore::exists(const StorageObject& object) const
{
    const std::string key = "hestia:object:" + object.id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest request(
        KeyValueStoreRequestMethod::STRING_EXISTS, query);
    const auto response = m_client->make_request(request);
    if (!response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_EXISTS: "
            + response->get_error().to_string());
    }
    return response->has_key();
}

void HsmKeyValueStore::remove(const StorageObject& object) const
{
    const std::string key = "hestia:object:" + object.id();
    const Metadata::Query query{key, ""};

    KeyValueStoreRequest string_request(
        KeyValueStoreRequestMethod::STRING_REMOVE, query);
    const auto string_response = m_client->make_request(string_request);
    if (!string_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store STRING_REMOVE: "
            + string_response->get_error().to_string());
    }

    KeyValueStoreRequest set_request(
        KeyValueStoreRequestMethod::SET_REMOVE, query);
    const auto set_response = m_client->make_request(set_request);
    if (!set_response->ok()) {
        throw std::runtime_error(
            "Error in kv_store SET_REMOVE: "
            + set_response->get_error().to_string());
    }
}

void HsmKeyValueStore::get(StorageTier& tier) const
{
    (void)tier;
}

void HsmKeyValueStore::put(const StorageTier& tier) const
{
    (void)tier;
}

void HsmKeyValueStore::list_objects(std::vector<StorageObject>& objects) const
{
    (void)objects;
}

void HsmKeyValueStore::list_tiers(std::vector<StorageTier>& tiers) const
{
    (void)tiers;
}

void HsmKeyValueStore::on_exception(
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

void HsmKeyValueStore::on_exception(
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
