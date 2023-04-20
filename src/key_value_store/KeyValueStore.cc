#include "KeyValueStore.h"

#include "KeyValueStoreClient.h"

KeyValueStore::KeyValueStore(std::unique_ptr<KeyValueStoreClient> client) :
    mClient(std::move(client))
{
}

KeyValueStore::~KeyValueStore() {}

ostk::ObjectStoreResponse::Ptr KeyValueStore::makeRequest(
    const ostk::ObjectStoreRequest& request) const noexcept
{
    return mClient->makeRequest(request);
}
