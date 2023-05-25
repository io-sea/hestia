#include "MotrHsmClient.h"

#include "MotrInterface.h"

namespace hestia {
MotrHsmClient::MotrHsmClient(std::unique_ptr<MotrInterface> interface) :
    m_motr_interface(std::move(interface))
{
    if (m_motr_interface == nullptr) {
        m_motr_interface = std::make_unique<MotrInterface>();
    }
}

void MotrHsmClient::intialize(const MotrConfig& config)
{
    m_motr_interface->initialize(config);
}

void MotrHsmClient::copy(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->copy(request);
}

void MotrHsmClient::get(
    const HsmObjectStoreRequest& request,
    StorageObject& object,
    Stream* stream) const
{
    m_motr_interface->get(request, object, stream);
}

void MotrHsmClient::move(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->move(request);
}

void MotrHsmClient::put(
    const HsmObjectStoreRequest& request, Stream* stream) const
{
    m_motr_interface->put(request, stream);
}

void MotrHsmClient::remove(const HsmObjectStoreRequest& request) const
{
    m_motr_interface->remove(request);
}
}  // namespace hestia