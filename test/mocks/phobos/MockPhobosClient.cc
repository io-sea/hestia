#include "MockPhobosClient.h"

#include "Logger.h"
#include "MockPhobosInterface.h"
#include "ProjectConfig.h"

#include "MockPhobosInterface.h"

#include <filesystem>

namespace hestia::mock {
MockPhobosClient::MockPhobosClient() :
    hestia::PhobosClient(
        hestia::PhobosInterface::create(MockPhobosInterface::create()))
{
}

MockPhobosClient::~MockPhobosClient()
{
    LOG_INFO("Destroyed");
}

MockPhobosClient::Ptr MockPhobosClient::create()
{
    return std::make_unique<MockPhobosClient>();
}

void MockPhobosClient::initialize(
    const std::string&,
    const std::string& cache_path,
    const Dictionary& config_data)
{
    if (config_data.has_map_item("root")) {
        auto root = config_data.get_map_item("root")->get_scalar();
        if (std::filesystem::path(root).is_relative()) {
            root = cache_path + "/" + root;
        }
        dynamic_cast<MockPhobosInterface*>(m_phobos_interface->impl())
            ->set_root(root);
    }
    if (config_data.has_map_item("redirect_location")) {
        set_redirect_location(
            config_data.get_map_item("redirect_location")->get_scalar());
    }
}

std::string MockPhobosClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::mock::MockPhobosClient";
}

void MockPhobosClient::set_redirect_location(const std::string& location)
{
    dynamic_cast<MockPhobosInterface*>(m_phobos_interface->impl())
        ->set_redirect_location(location);
}

}  // namespace hestia::mock