#pragma once

#include <memory>

class HsmObjectStoreResponse;

class HsmMiddlewareResponse
{
public:
    using Ptr = std::unique_ptr<HsmMiddlewareResponse>;
    virtual ~HsmMiddlewareResponse();

    HsmObjectStoreResponse* getObjectStoreResponse() const;

    virtual void setObjectStoreRespose(std::unique_ptr<HsmObjectStoreResponse> objectStoreResponse) = 0;

protected:
    std::unique_ptr<HsmObjectStoreResponse> mObjectStoreResponse;
};