#include "HsmMiddlewareResponse.h"

#include "HsmObjectStoreResponse.h"

namespace hestia {
HsmMiddlewareResponse::~HsmMiddlewareResponse() {}

HsmObjectStoreResponse* HsmMiddlewareResponse::get_object_store_response() const
{
    return m_object_store_response.get();
}
}  // namespace hestia