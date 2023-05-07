#include "KeyValueStoreClient.h"

namespace hestia {
ObjectStoreResponse::Ptr KeyValueStoreClient::make_request(
    const ObjectStoreRequest& request) const noexcept
{
    (void)request;
    return nullptr;
}

KeyValueStoreClient::~KeyValueStoreClient() {}
}  // namespace hestia
