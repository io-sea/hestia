#include "CopyToolResponse.h"

CopyToolResponse::CopyToolResponse() : HsmMiddlewareResponse() {}

CopyToolResponse::Ptr CopyToolResponse::Create()
{
    return std::make_unique<CopyToolResponse>();
}

bool CopyToolResponse::ok() const
{
    return mStatus != Status::ERROR;
};

void CopyToolResponse::setError(const CopyToolError& error)
{
    mError = error;
}

void CopyToolResponse::setObjectStoreRespose(
    HsmObjectStoreResponse::Ptr objectStoreResponse)
{
    mObjectStoreResponse = std::move(objectStoreResponse);
    if (!mObjectStoreResponse->ok()) {
        const std::string msg = "Object store failed with error: "
                                + objectStoreResponse->getError().toString();
        setError({CopyToolError::Code::ERROR_OBJECT_STORE, msg});
    }
}