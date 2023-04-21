#include "HsmMiddlewareResponse.h"

#include "HsmObjectStoreResponse.h"

HsmMiddlewareResponse::~HsmMiddlewareResponse() {}

HsmObjectStoreResponse* HsmMiddlewareResponse::get_object_store_response() const
{
    return m_object_store_response.get();
}
