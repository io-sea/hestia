#include "HsmObjectStoreClient.h"

#include <ostk/Logger.h>

HsmObjectStoreResponse::Ptr HsmObjectStoreClient::make_request(
    const HsmObjectStoreRequest& request, ostk::Stream* stream) const noexcept
{
    auto response = HsmObjectStoreResponse::create(request);
    switch (request.method()) {
        case HsmObjectStoreRequestMethod::GET:
            try {
                get(request, response->object(), stream);
            }
            catch (std::exception& e) {
                on_exception(request, response.get(), e.what());
                return response;
            }
            catch (...) {
                on_exception(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::PUT:
            try {
                put(request, stream);
            }
            catch (std::exception& e) {
                on_exception(request, response.get(), e.what());
                return response;
            }
            catch (...) {
                on_exception(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::MOVE:
            try {
                move(request);
            }
            catch (std::exception& e) {
                on_exception(request, response.get(), e.what());
                return response;
            }
            catch (...) {
                on_exception(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::COPY:
            try {
                copy(request);
            }
            catch (std::exception& e) {
                on_exception(request, response.get(), e.what());
                return response;
            }
            catch (...) {
                on_exception(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::REMOVE:
            try {
                remove(request);
            }
            catch (std::exception& e) {
                on_exception(request, response.get(), e.what());
                return response;
            }
            catch (...) {
                on_exception(request, response.get());
                return response;
            }
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

ostk::ObjectStoreResponse::Ptr HsmObjectStoreClient::make_request(
    const ostk::ObjectStoreRequest& request,
    ostk::Stream* stream) const noexcept
{
    auto response = ostk::ObjectStoreResponse::create(request);
    if (!HsmObjectStoreRequest::is_hsm_supported_method(request.method())) {
        const std::string msg =
            "Requested unsupported type for base object operation in HSM object store client.";
        response->on_error(
            {ostk::ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg});
        return response;
    }

    HsmObjectStoreRequest hsm_request(request);
    auto hsm_response = make_request(hsm_request, stream);
    return HsmObjectStoreResponse::to_base_response(
        hsm_request, hsm_response.get());
}

bool HsmObjectStoreClient::exists(const ostk::StorageObject& object) const
{
    throw std::runtime_error("Exists operation not supported for hsm objects");
}

void HsmObjectStoreClient::list(
    const ostk::Metadata::Query& query,
    std::vector<ostk::StorageObject>& fetched) const
{
    throw std::runtime_error("List operation not supported for hsm objects");
}

void HsmObjectStoreClient::get(
    ostk::StorageObject& object,
    const ostk::Extent& extent,
    ostk::Stream* stream) const
{
    HsmObjectStoreRequest request(
        object.m_id, HsmObjectStoreRequestMethod::GET);
    request.set_extent(extent);
    if (const auto response = make_request(request, stream); !response->ok()) {
        const std::string msg =
            "Error in single tier GET: " + response->get_error().to_string();
        throw ostk::ObjectStoreException(
            {ostk::ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::put(
    const ostk::StorageObject& object,
    const ostk::Extent& extent,
    ostk::Stream* stream) const
{
    HsmObjectStoreRequest request(
        object.m_id, HsmObjectStoreRequestMethod::PUT);
    request.set_extent(extent);
    if (const auto response = make_request(request, stream); !response->ok()) {
        const std::string msg =
            "Error in single tier PUT: " + response->get_error().to_string();
        throw ostk::ObjectStoreException(
            {ostk::ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::remove(const ostk::StorageObject& object) const
{
    HsmObjectStoreRequest request(
        object.m_id, HsmObjectStoreRequestMethod::REMOVE);
    if (const auto response = make_request(request); !response->ok()) {
        const std::string msg =
            "Error in single tier REMOVE: " + response->get_error().to_string();
        throw ostk::ObjectStoreException(
            {ostk::ObjectStoreErrorCode::ERROR, msg});
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