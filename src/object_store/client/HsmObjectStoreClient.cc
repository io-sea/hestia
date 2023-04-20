#include "HsmObjectStoreClient.h"

#include <ostk/Logger.h>

HsmObjectStoreResponse::Ptr HsmObjectStoreClient::makeRequest(const HsmObjectStoreRequest& request, ostk::Stream* stream) const noexcept
{
    auto response = HsmObjectStoreResponse::Create(request);
    switch(request.method())
    {
        case HsmObjectStoreRequestMethod::GET:
            try
            {
                get(request, response->object(), stream);
            }
            catch(std::exception &e)
            {
                onException(request, response.get(), e.what());
                return response;
            }
            catch(...)
            {
                onException(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::PUT:
            try
            {
                put(request, stream);
            }
            catch(std::exception &e)
            {
                onException(request, response.get(), e.what());
                return response;
            }
            catch(...)
            {
                onException(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::MOVE:
            try
            {
                move(request);
            }
            catch(std::exception &e)
            {
                onException(request, response.get(), e.what());
                return response;
            }
            catch(...)
            {
                onException(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::COPY:
            try
            {
                copy(request);
            }
            catch(std::exception &e)
            {
                onException(request, response.get(), e.what());
                return response;
            }
            catch(...)
            {
                onException(request, response.get());
                return response;
            }
            break;
        case HsmObjectStoreRequestMethod::REMOVE:
            try
            {
                remove(request);
            }
            catch(std::exception &e)
            {
                onException(request, response.get(), e.what());
                return response;
            }
            catch(...)
            {
                onException(request, response.get());
                return response;
            }
            break;
        default:
            const std::string msg = "Method: " + request.methodAsString() + " not supported";
            const HsmObjectStoreError error(HsmObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg);
            LOG_ERROR("Error: " << error);
            response->onError(error);
            return response;
    }
    return response;
}

ostk::ObjectStoreResponse::Ptr HsmObjectStoreClient::makeRequest(const ostk::ObjectStoreRequest& request, ostk::Stream* stream) const noexcept
{
    auto response = ostk::ObjectStoreResponse::Create(request);
    if (!HsmObjectStoreRequest::isHsmSupportedMethod(request.method()))
    {
        const std::string msg = "Requested unsupported type for base object operation in HSM object store client.";
        response->onError({ostk::ObjectStoreErrorCode::UNSUPPORTED_REQUEST_METHOD, msg});
        return response;
    }

    HsmObjectStoreRequest hsm_request(request);
    auto hsm_response = makeRequest(hsm_request, stream);
    return HsmObjectStoreResponse::toBaseResponse(hsm_request, hsm_response.get());
}

bool HsmObjectStoreClient::exists(const ostk::StorageObject& object) const
{
    throw std::runtime_error("Exists operation not supported for hsm objects");
}

void HsmObjectStoreClient::list(const ostk::Metadata::Query& query, std::vector<ostk::StorageObject>& fetched) const
{
    throw std::runtime_error("List operation not supported for hsm objects");
}

void HsmObjectStoreClient::get(ostk::StorageObject& object, const ostk::Extent& extent, ostk::Stream* stream) const
{
    HsmObjectStoreRequest request(object.mId, HsmObjectStoreRequestMethod::GET);
    request.setExtent(extent);
    if (const auto response = makeRequest(request, stream); !response->ok())
    {
        const std::string msg = "Error in single tier GET: " + response->getError().toString();
        throw ostk::ObjectStoreException({ostk::ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::put(const ostk::StorageObject& object, const ostk::Extent& extent, ostk::Stream* stream) const
{
    HsmObjectStoreRequest request(object.mId, HsmObjectStoreRequestMethod::PUT);
    request.setExtent(extent);
    if (const auto response = makeRequest(request, stream); !response->ok())
    {
        const std::string msg = "Error in single tier PUT: " + response->getError().toString();
        throw ostk::ObjectStoreException({ostk::ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::remove(const ostk::StorageObject& object) const
{
    HsmObjectStoreRequest request(object.mId, HsmObjectStoreRequestMethod::REMOVE);
    if (const auto response = makeRequest(request); !response->ok())
    {
        const std::string msg = "Error in single tier REMOVE: " + response->getError().toString();
        throw ostk::ObjectStoreException({ostk::ObjectStoreErrorCode::ERROR, msg});
    }
}

void HsmObjectStoreClient::onException(const HsmObjectStoreRequest& request, HsmObjectStoreResponse* response, const std::string& message) const
{
    if (!message.empty())
    {
        const std::string msg = "Exception in " + request.methodAsString() + " method: " + message;
        const HsmObjectStoreError error(HsmObjectStoreErrorCode::STL_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->onError(error);
    }
    else
    {
        const std::string msg = "Uknown Exception in " + request.methodAsString() + " method";
        const HsmObjectStoreError error(HsmObjectStoreErrorCode::UNKNOWN_EXCEPTION, msg);
        LOG_ERROR("Error: " << error);
        response->onError(error);
    }
}