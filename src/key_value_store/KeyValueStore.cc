#include "KeyValueStore.h"

#include "KeyValueStoreClient.h"

KeyValueStore::KeyValueStore(std::unique_ptr<KeyValueStoreClient> client) :
    m_client(std::move(client))
{
}

KeyValueStore::~KeyValueStore() {}

ostk::ObjectStoreResponse::Ptr KeyValueStore::make_request(
    const ostk::ObjectStoreRequest& request) const noexcept
{
    return m_client->make_request(request);
}
