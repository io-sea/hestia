#include "MockCrudService.h"

#include "KeyValueCrudClient.h"
#include "MockModel.h"

namespace hestia::mock {

MockCrudService::MockCrudService(
    const ServiceConfig& config, std::unique_ptr<CrudClient> client) :
    CrudService(config, std::move(client))
{
}

MockCrudService::Ptr MockCrudService::create()
{
    auto id_generator    = std::make_unique<hestia::mock::MockIdGenerator>();
    auto time_provider   = std::make_unique<hestia::mock::MockTimeProvider>();
    auto kv_store_client = std::make_unique<InMemoryKeyValueStoreClient>();

    CrudClientConfig config;
    auto crud_client = std::make_unique<KeyValueCrudClient>(
        config, std::make_unique<ModelSerializer>(MockModel::create_factory()),
        kv_store_client.get(), id_generator.get(), time_provider.get());

    auto service = std::make_unique<MockCrudService>(
        hestia::ServiceConfig{}, std::move(crud_client));

    service->m_mock_id_generator  = std::move(id_generator);
    service->m_mock_time_provider = std::move(time_provider);
    service->m_kv_store_client    = std::move(kv_store_client);
    return service;
}

MockCrudService::Ptr MockCrudService::create_one_to_one(
    KeyValueStoreClient* kv_store_client)
{
    auto id_generator  = std::make_unique<hestia::mock::MockIdGenerator>();
    auto time_provider = std::make_unique<hestia::mock::MockTimeProvider>();

    CrudClientConfig config;
    auto crud_client = std::make_unique<KeyValueCrudClient>(
        config,
        std::make_unique<ModelSerializer>(MockOneToOneModel::create_factory()),
        kv_store_client, id_generator.get(), time_provider.get());

    auto service = std::make_unique<MockCrudService>(
        hestia::ServiceConfig{}, std::move(crud_client));

    service->m_mock_id_generator  = std::move(id_generator);
    service->m_mock_time_provider = std::move(time_provider);
    return service;
}

MockCrudService::Ptr MockCrudService::create_mock_with_parent(
    KeyValueStoreClient* kv_store_client)
{
    auto id_generator  = std::make_unique<hestia::mock::MockIdGenerator>();
    auto time_provider = std::make_unique<hestia::mock::MockTimeProvider>();

    CrudClientConfig config;
    auto crud_client = std::make_unique<KeyValueCrudClient>(
        config,
        std::make_unique<ModelSerializer>(
            MockModelWithParent::create_factory()),
        kv_store_client, id_generator.get(), time_provider.get());

    auto service = std::make_unique<MockCrudService>(
        hestia::ServiceConfig{}, std::move(crud_client));

    service->m_mock_id_generator  = std::move(id_generator);
    service->m_mock_time_provider = std::move(time_provider);
    return service;
}

MockCrudService::Ptr MockCrudService::create_for_parent(
    KeyValueStoreClient* kv_store_client)
{
    auto id_generator  = std::make_unique<hestia::mock::MockIdGenerator>();
    auto time_provider = std::make_unique<hestia::mock::MockTimeProvider>();

    CrudClientConfig config;
    auto crud_client = std::make_unique<KeyValueCrudClient>(
        config,
        std::make_unique<ModelSerializer>(MockParentModel::create_factory()),
        kv_store_client, id_generator.get(), time_provider.get());

    auto service = std::make_unique<MockCrudService>(
        hestia::ServiceConfig{}, std::move(crud_client));

    service->m_mock_id_generator  = std::move(id_generator);
    service->m_mock_time_provider = std::move(time_provider);
    return service;
}

MockCrudService::Ptr MockCrudService::create_many_many_parent(
    KeyValueStoreClient* kv_store_client)
{
    auto id_generator  = std::make_unique<hestia::mock::MockIdGenerator>();
    auto time_provider = std::make_unique<hestia::mock::MockTimeProvider>();

    CrudClientConfig config;
    auto crud_client = std::make_unique<KeyValueCrudClient>(
        config,
        std::make_unique<ModelSerializer>(
            MockManyToManyTargetModel::create_factory()),
        kv_store_client, id_generator.get(), time_provider.get());

    auto service = std::make_unique<MockCrudService>(
        hestia::ServiceConfig{}, std::move(crud_client));

    service->m_mock_id_generator  = std::move(id_generator);
    service->m_mock_time_provider = std::move(time_provider);
    return service;
}
}  // namespace hestia::mock