#include "CopyToolResponse.h"

CopyToolResponse::CopyToolResponse() : HsmMiddlewareResponse() {}

CopyToolResponse::Ptr CopyToolResponse::create()
{
    return std::make_unique<CopyToolResponse>();
}

bool CopyToolResponse::ok() const
{
    return mStatus != Status::ERROR;
};

void CopyToolResponse::set_error(const CopyToolError& error)
{
    mError = error;
}

void CopyToolResponse::set_object_store_respose(
    HsmObjectStoreResponse::Ptr object_store_response)
{
    mObjectStoreResponse = std::move(objectStoreResponse);
    if (!mObjectStoreResponse->ok()) {
        const std::string msg = "Object store failed with error: "
                                + objectStoreResponse->getError().toString();
        setError({CopyToolError::Code::ERROR_OBJECT_STORE, msg});
    }
}