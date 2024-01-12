#include "MockPhobosClient.h"

#include "MockPhobosInterface.h"
#include "ProjectConfig.h"

#include "MockPhobosInterface.h"

namespace hestia::mock {
MockPhobosClient::MockPhobosClient() :
    hestia::PhobosClient(
        hestia::PhobosInterface::create(MockPhobosInterface::create()))
{
}

MockPhobosClient::Ptr MockPhobosClient::create()
{
    return std::make_unique<MockPhobosClient>();
}

std::string MockPhobosClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::mock::MockPhobosClient";
}

void MockPhobosClient::set_redirect_location(const std::string& location)
{
    dynamic_cast<MockPhobosInterface*>(m_phobos_interface->impl())->set_redirect_location(location);
}

}  // namespace hestia::mock