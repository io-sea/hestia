#pragma once

#include "CrudService.h"
#include "InMemoryKeyValueStoreClient.h"
#include "Model.h"
#include "StringAdapter.h"

#include "MockIdGenerator.h"
#include "MockTimeProvider.h"

namespace hestia::mock {
class MockCrudService : public CrudService {
  public:
    MockCrudService(
        const ServiceConfig& config, std::unique_ptr<CrudClient> client);

    using Ptr = std::unique_ptr<MockCrudService>;

    static Ptr create();

    static Ptr create_mock_with_parent(KeyValueStoreClient* kv_store_client);

    static Ptr create_many_many_parent(KeyValueStoreClient* kv_store_client);

    static Ptr create_for_parent(KeyValueStoreClient* kv_store_client);

    std::unique_ptr<InMemoryKeyValueStoreClient> m_kv_store_client;
    std::unique_ptr<hestia::mock::MockIdGenerator> m_mock_id_generator;
    std::unique_ptr<hestia::mock::MockTimeProvider> m_mock_time_provider;
};
}  // namespace hestia::mock