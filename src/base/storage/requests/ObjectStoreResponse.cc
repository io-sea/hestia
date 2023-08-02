#include "ObjectStoreResponse.h"

namespace hestia {

ObjectStoreResponse::ObjectStoreResponse(
    const BaseObjectStoreRequest& request, const std::string& store_id) :
    BaseObjectStoreResponse(request, store_id)
{
}

ObjectStoreResponse::Ptr ObjectStoreResponse::create(
    const BaseObjectStoreRequest& request, const std::string& store_id)
{
    return std::make_unique<ObjectStoreResponse>(request, store_id);
}

}  // namespace hestia