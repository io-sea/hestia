#pragma once

#include <ostk/ObjectStoreResponse.h>

class KeyValueStoreClient;

class KeyValueStore 
{
public:
    KeyValueStore(std::unique_ptr<KeyValueStoreClient> client);

    virtual ~KeyValueStore();
 
    [[nodiscard]] ostk::ObjectStoreResponse::Ptr makeRequest(const ostk::ObjectStoreRequest& request) const noexcept;

private:
  std::unique_ptr<KeyValueStoreClient> mClient;
};
