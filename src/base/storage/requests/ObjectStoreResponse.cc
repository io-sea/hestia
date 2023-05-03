#include "ObjectStoreResponse.h"

namespace hestia {

ObjectStoreResponse::ObjectStoreResponse(
    const BaseObjectStoreRequest& request) :
    BaseObjectStoreResponse(request)
{
}

ObjectStoreResponse::Ptr ObjectStoreResponse::create(
    const BaseObjectStoreRequest& request)
{
    return std::make_unique<ObjectStoreResponse>(request);
}
}  // namespace hestia