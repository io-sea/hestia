#include "KeyValueStore.h"

#include "KeyValueStoreClient.h"

namespace hestia {
KeyValueStore::KeyValueStore(std::unique_ptr<KeyValueStoreClient> client) :
    m_client(std::move(client))
{
}

KeyValueStore::~KeyValueStore() {}

ObjectStoreResponse::Ptr KeyValueStore::make_request(
    const ObjectStoreRequest& request) const noexcept
{
    return m_client->make_request(request);
}
}  // namespace hestia
