#include "HsmObjectStoreClient.h"

#include "Logger.h"

#define CATCH_FLOW()                                                           \
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

HsmObjectStoreClient::~HsmObjectStoreClient() {}

HsmObjectStoreResponse::Ptr HsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, Stream* stream) const noexcept
{
    auto response = HsmObjectStoreResponse::create(request);
    switch (request.method()) {
        case HsmObjectStoreRequestMethod::GET:
            try {
                get(request, response->object(), stream);
            }
            CATCH_FLOW();
            break;
        case HsmObjectStoreRequestMethod::PUT:
            try {
                put(request, stream);
            }
            CATCH_FLOW();
            break;
        case HsmObjectStoreRequestMethod::MOVE:
            try {
                move(request);
            }
            CATCH_FLOW();
            break;
        case HsmObjectStoreRequestMethod::COPY:
            try {
                copy(request);
            }
            CATCH_FLOW();
            break;
        case HsmObjectStoreRequestMethod::REMOVE:
            try {
                remove(request);
            }
            CATCH_FLOW();
            break;
        case HsmObjectStoreRequestMethod::REMOVE_ALL:
            LOG_ERROR("Not implemented yet: ");
            break;
        case HsmObjectStoreRequestMethod::EXISTS:
            LOG_ERROR("Not implemented yet: ");
            break;
        default:
            const std::string msg =
                "Method: " + request.method_as_string() + " not supported";
            const HsmObjectStoreError error(
                HsmObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->on_error(error);
            return response;
    }
    return response;
}

ObjectStoreResponse::Ptr HsmObjectStoreClient::make_request(
    const ObjectStoreRequest& request, Stream* stream) const noexcept
{
    auto response = ObjectStoreResponse::create(request);
    if (!HsmObjectStoreRequest::is_hsm_supported_method(request.method())) {
        const std::string msg =
            "Requested unsupported type for base object operation in HSM object store client.";
        response->on_error(
            {ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg});
        return response;
    }

    HsmObjectStoreRequest hsm_request(request);
    auto hsm_response = make_request(hsm_request, stream);
    return HsmObjectStoreResponse::to_base_response(
        hsm_request, hsm_response.get());
}

bool HsmObjectStoreClient::exists(const StorageObject& object) const
{
    (void)object;
    throw std::runtime_error("Exists operation not supported for hsm objects");
}

void HsmObjectStoreClient::list(
    const Metadata::Query& query, std::vector<StorageObject>& fetched) const
{
    (void)query;
    (void)fetched;
    throw std::runtime_error("List operation not supported for hsm objects");
}

void HsmObjectStoreClient::get(
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    HsmObjectStoreRequest request(
        object.id(), HsmObjectStoreRequestMethod::GET);
    request.set_extent(extent);
    if (const auto response = make_request(request, stream); !response->ok()) {
        const std::string msg =
            "Error in single tier GET: " + response->get_error().to_string();
        throw ObjectStoreException({ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    HsmObjectStoreRequest request(
        object.id(), HsmObjectStoreRequestMethod::PUT);
    request.set_extent(extent);
    if (const auto response = make_request(request, stream); !response->ok()) {
        const std::string msg =
            "Error in single tier PUT: " + response->get_error().to_string();
        throw ObjectStoreException({ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::remove(const StorageObject& object) const
{
    HsmObjectStoreRequest request(
        object.id(), HsmObjectStoreRequestMethod::REMOVE);
    if (const auto response = make_request(request); !response->ok()) {
        const std::string msg =
            "Error in single tier REMOVE: " + response->get_error().to_string();
        throw ObjectStoreException({ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::on_exception(
    const HsmObjectStoreRequest& request,
    HsmObjectStoreResponse* response,
    const std::string& message) const
{
    if (!message.empty()) {
        const std::string msg = "Exception in " + request.method_as_string()
                                + " method: " + message;
        const HsmObjectStoreError error(
            HsmObjectStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
    else {
        const std::string msg =
            "Uknown Exception in " + request.method_as_string() + " method";
        const HsmObjectStoreError error(
            HsmObjectStoreErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->on_error(error);
    }
}
}  // namespace hestia