#include "HsmMiddlewareResponse.h"

#include "HsmObjectStoreResponse.h"

HsmMiddlewareResponse::~HsmMiddlewareResponse()
{

}

HsmObjectStoreResponse* HsmMiddlewareResponse::getObjectStoreResponse() const
{
    return mObjectStoreResponse.get();
}



