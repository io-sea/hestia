#pragma once

#include "CrudServiceBackend.h"
#include "CrudServiceWithUser.h"
#include "HttpCrudClient.h"
#include "IdGenerator.h"
#include "KeyValueCrudClient.h"
#include "ModelSerializer.h"

#include <cassert>
#include <memory>

namespace hestia {

template<typename ItemT>
class CrudServiceFactory {
  public:
    static std::unique_ptr<CrudService> create(
        const ServiceConfig& config,
        CrudServiceBackend* backend,
        EventFeed* event_feed,
        UserService* user_service                    = nullptr,
        std::unique_ptr<IdGenerator> id_generator_in = {})
    {
        assert(backend != nullptr);

        auto serializer = std::make_unique<ModelSerializer>(
            std::make_unique<TypedModelFactory<ItemT>>());

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

            if (id_generator_in) {
                id_generator = std::move(id_generator_in);
            }
            else {
                id_generator = std::make_unique<DefaultIdGenerator>();
            }
            crud_client = std::make_unique<KeyValueCrudClient>(
                crud_client_config, std::move(serializer), kv_backend->m_client,
                id_generator.get());
        }
        else if (backend->get_type() == CrudServiceBackend::Type::HTTP_REST) {
            auto http_backend =
                dynamic_cast<HttpRestCrudServiceBackend*>(backend);
            if (http_backend == nullptr) {
                throw std::runtime_error(
                    "Failed to convert to http service backend");
            }

            crud_client = std::make_unique<HttpCrudClient>(
                crud_client_config, std::move(serializer),
                http_backend->m_client);
        }

        if (user_service == nullptr) {
            return std::make_unique<CrudServiceWithUser>(
                config, std::move(crud_client), user_service, event_feed,
                std::move(id_generator));
        }
        else {
            return std::make_unique<CrudService>(
                config, std::move(crud_client), event_feed,
                std::move(id_generator));
        }
    }
};

}  // namespace hestia
