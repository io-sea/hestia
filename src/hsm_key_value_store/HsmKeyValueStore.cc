#include "HsmKeyValueStore.h"

#include "KeyValueStoreClient.h"

namespace hestia {
HsmKeyValueStore::HsmKeyValueStore(
    std::unique_ptr<KeyValueStoreClient> client) :
    m_client(std::move(client))
{
}

HsmKeyValueStore::~HsmKeyValueStore() {}

ObjectStoreResponse::Ptr HsmKeyValueStore::make_request(
    const ObjectStoreRequest& request) const noexcept
{
    return m_client->make_request(request);
}
}  // namespace hestia
