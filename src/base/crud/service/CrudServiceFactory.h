#pragma once

#include "CrudServiceBackend.h"
#include "CrudServiceWithUser.h"
#include "HttpCrudClient.h"
#include "IdGenerator.h"
#include "KeyValueCrudClient.h"
#include "StringAdapter.h"

#include <cassert>
#include <memory>

namespace hestia {

template<typename ItemT>
class CrudServiceFactory {
  public:
    static std::unique_ptr<CrudService> create(
        const ServiceConfig& config,
        CrudServiceBackend* backend,
        UserService* user_service = nullptr)
    {
        assert(backend != nullptr);

        auto item_factory     = std::make_unique<TypedModelFactory<ItemT>>();
        auto item_factory_raw = item_factory.get();

        auto adapter_collection =
            std::make_unique<AdapterCollection>(std::move(item_factory));
        adapter_collection->add_adapter(
            CrudAttributes::to_string(CrudAttributes::Format::JSON),
            std::make_unique<JsonAdapter>(item_factory_raw));
        adapter_collection->add_adapter(
            CrudAttributes::to_string(CrudAttributes::Format::KV_PAIR),
            std::make_unique<KeyValueAdapter>(item_factory_raw));

        CrudClientConfig crud_client_config;
        crud_client_config.m_prefix   = config.m_global_prefix;
        crud_client_config.m_endpoint = config.m_endpoint;

        std::unique_ptr<CrudClient> crud_client;
        std::unique_ptr<IdGenerator> id_generator;

        if (backend->get_type() == CrudServiceBackend::Type::KEY_VALUE_STORE) {
            auto kv_backend =
                dynamic_cast<KeyValueStoreCrudServiceBackend*>(backend);
            if (kv_backend == nullptr) {
                throw std::runtime_error(
                    "Failed to convert to kv service backend");
            }

            assert(kv_backend->m_client != nullptr);

            id_generator = std::make_unique<DefaultIdGenerator>();
            crud_client  = std::make_unique<KeyValueCrudClient>(
                crud_client_config, std::move(adapter_collection),
                kv_backend->m_client, id_generator.get());
        }
        else if (backend->get_type() == CrudServiceBackend::Type::HTTP_REST) {
            auto http_backend =
                dynamic_cast<HttpRestCrudServiceBackend*>(backend);
            if (http_backend == nullptr) {
                throw std::runtime_error(
                    "Failed to convert to http service backend");
            }

            crud_client = std::make_unique<HttpCrudClient>(
                crud_client_config, std::move(adapter_collection),
                http_backend->m_client);
        }

        if (user_service == nullptr) {
            return std::make_unique<CrudServiceWithUser>(
                config, std::move(crud_client), user_service,
                std::move(id_generator));
        }
        else {
            return std::make_unique<CrudService>(
                config, std::move(crud_client), std::move(id_generator));
        }
    }
};

}  // namespace hestia
