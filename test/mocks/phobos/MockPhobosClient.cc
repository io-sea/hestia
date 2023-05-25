#include "MockPhobosClient.h"

#include "MockPhobosInterface.h"
#include "ProjectConfig.h"

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
}  // namespace hestia::mock