#include "HsmObjectStoreClientRegistry.h"

#include "FileHsmObjectStoreClient.h"

#include "FileObjectStoreClient.h"

#include "Logger.h"

namespace hestia {

ObjectStorePluginHandler::ObjectStorePluginHandler(
    const std::vector<std::filesystem::directory_entry>& search_paths) :
    m_search_paths(search_paths)
{
}

bool ObjectStorePluginHandler::has_plugin(const std::string& identifier)
{
    (void)identifier;
    return false;
}

ObjectStoreClient::Ptr ObjectStorePluginHandler::get_client(
    const HsmObjectStoreClientSpec& client_spec) const
{
    (void)client_spec;
    return nullptr;
}

HsmObjectStoreClientRegistry::HsmObjectStoreClientRegistry(
    ObjectStorePluginHandler::Ptr plugin_handler) :
    m_plugin_handler(std::move(plugin_handler))
{
}

bool HsmObjectStoreClientRegistry::is_client_type_available(
    const HsmObjectStoreClientSpec& client_spec) const
{
    if (client_spec.m_source == HsmObjectStoreClientSpec::Source::BUILT_IN) {
        return true;
    }
    return m_plugin_handler->has_plugin(client_spec.m_identifier);
}

ObjectStoreClient::Ptr HsmObjectStoreClientRegistry::get_client(
    const HsmObjectStoreClientSpec& client_spec) const
{
    if (client_spec.m_source == HsmObjectStoreClientSpec::Source::BUILT_IN) {
        if (client_spec.m_identifier
            == FileHsmObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileHsmObjectStoreClient");
            return FileHsmObjectStoreClient::create();
        }
        else if (
            client_spec.m_identifier
            == hestia::FileObjectStoreClient::get_registry_identifier()) {
            LOG_INFO("Setting up FileObjectStoreClient");
            return hestia::FileObjectStoreClient::create();
        }
    }
    else {
        return m_plugin_handler->get_client(client_spec);
    }
    LOG_ERROR(
        "Requested client spec not recognized - returning empty client: "
        + client_spec.to_string());

    return nullptr;
}
}  // namespace hestia
