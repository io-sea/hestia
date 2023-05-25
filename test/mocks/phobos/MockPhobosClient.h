#pragma once

#include "PhobosClient.h"

namespace hestia::mock {
class MockPhobosClient : public hestia::PhobosClient {
  public:
    using Ptr = std::unique_ptr<MockPhobosClient>;

    MockPhobosClient();

    virtual ~MockPhobosClient() = default;

    static Ptr create();

    static std::string get_registry_identifier();
};
}  // namespace hestia::mock