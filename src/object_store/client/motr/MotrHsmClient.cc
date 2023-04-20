#include "MotrHsmClient.h"

#include "MotrInterface.h"

MotrHsmClient::MotrHsmClient()
    : mMotrInterface(std::make_unique<MotrInterface>())
{

}

void MotrHsmClient::intialize(const MotrConfig& config)
{
    mMotrInterface->initialize(config);
}

void MotrHsmClient::copy(const HsmObjectStoreRequest& request) const
{
    mMotrInterface->copy(request);
}

void MotrHsmClient::get(const HsmObjectStoreRequest& request, ostk::StorageObject& object, ostk::Stream* stream) const
{
    mMotrInterface->get(request, object, stream);
}

void MotrHsmClient::move(const HsmObjectStoreRequest& request) const
{
    mMotrInterface->move(request);
}

void MotrHsmClient::put(const HsmObjectStoreRequest& request, ostk::Stream* stream) const
{
    mMotrInterface->put(request, stream);
}

void MotrHsmClient::remove(const HsmObjectStoreRequest& request) const
{
    mMotrInterface->remove(request);
}