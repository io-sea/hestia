#include "ObjectStoreClientFactory.h"

#include "FileHsmObjectStoreClient.h"
#include "FileObjectStoreClient.h"
#include "InMemoryHsmObjectStoreClient.h"
#include "InMemoryObjectStoreClient.h"
#include "S3ObjectStoreClient.h"

#include "ErrorUtils.h"
#include "Logger.h"

namespace hestia {

ObjectStoreClientFactory::ObjectStoreClientFactory(
    ObjectStorePluginHandler::Ptr plugin_handler) :
    m_plugin_handler(std::move(plugin_handler))
{
}

ObjectStoreClientFactory::Ptr ObjectStoreClientFactory::create(
    ObjectStorePluginHandler::Ptr plugin_handler)
{
    return std::make_unique<ObjectStoreClientFactory>(
        std::move(plugin_handler));
}

bool ObjectStoreClientFactory::is_client_type_available(
    const ObjectStoreBackend& client_spec) const
{
    if (client_spec.is_built_in()) {
        return true;
    }
    return m_plugin_handler->has_plugin(client_spec);
}

ObjectStoreClient::Ptr ObjectStoreClientFactory::get_client(
    const ObjectStoreBackend& client_spec, S3Client* s3_client) const
{
    if (client_spec.is_built_in()) {
        if (client_spec.get_backend() == ObjectStoreBackend::Type::FILE_HSM) {
            LOG_INFO("Setting up FileHsmObjectStoreClient");
            return FileHsmObjectStoreClient::create();
        }
        else if (client_spec.get_backend() == ObjectStoreBackend::Type::FILE) {
            LOG_INFO("Setting up FileObjectStoreClient");
            return FileObjectStoreClient::create();
        }
        else if (
            client_spec.get_backend() == ObjectStoreBackend::Type::MEMORY_HSM) {
            LOG_INFO("Setting up InMemoryHsmObjectStoreClient");
            return InMemoryHsmObjectStoreClient::create();
        }
        else if (
            client_spec.get_backend() == ObjectStoreBackend::Type::MEMORY) {
            LOG_INFO("Setting up InMemoryObjectStoreClient");
            return InMemoryObjectStoreClient::create();
        }
        else if (client_spec.get_backend() == ObjectStoreBackend::Type::S3) {
            LOG_INFO("Setting up S3ObjectStoreClient");
            return S3ObjectStoreClient::create(s3_client);
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());

    return nullptr;
}

ObjectStoreClientPlugin::Ptr ObjectStoreClientFactory::get_client_from_plugin(
    const ObjectStoreBackend& client_spec) const
{
    if (client_spec.is_plugin() || client_spec.is_mock()) {
        if (!client_spec.is_hsm()) {
            return m_plugin_handler->get_object_store_plugin(client_spec);
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());
    return nullptr;
}

HsmObjectStoreClientPlugin::Ptr
ObjectStoreClientFactory::get_hsm_client_from_plugin(
    const ObjectStoreBackend& client_spec) const
{
    if (client_spec.is_plugin() || client_spec.is_mock()) {
        if (client_spec.is_hsm()) {
            return m_plugin_handler->get_hsm_object_store_plugin(client_spec);
        }
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());
    return nullptr;
}

}  // namespace hestia
