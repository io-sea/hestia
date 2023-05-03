#pragma once

#include "ObjectStoreResponse.h"

namespace hestia {
class KeyValueStoreClient;

class KeyValueStore {
  public:
    KeyValueStore(std::unique_ptr<KeyValueStoreClient> client);

    virtual ~KeyValueStore();

    [[nodiscard]] ObjectStoreResponse::Ptr make_request(
        const ObjectStoreRequest& request) const noexcept;

  private:
    std::unique_ptr<KeyValueStoreClient> m_client;
};
}  // namespace hestia
