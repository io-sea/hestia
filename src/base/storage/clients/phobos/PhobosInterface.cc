#include "PhobosInterface.h"

#include "IPhobosInterfaceImpl.h"

#ifdef HAS_PHOBOS
#include "PhobosInterfaceImpl.h"
#endif

#include <stdexcept>

namespace hestia {
PhobosInterface::PhobosInterface(IPhobosInterfaceImpl::Ptr impl) :
    m_impl(std::move(impl))
{
#ifdef HAS_PHOBOS
    if (!m_impl) {
        m_impl = std::make_unique<PhobosInterfaceImpl>();
    }
#endif

    if (!m_impl) {
        throw std::runtime_error(
            "Phobos interface has no implementation - phobos plugin not found.");
    }
}

PhobosInterface::Ptr PhobosInterface::create(IPhobosInterfaceImpl::Ptr impl)
{
    return std::make_unique<PhobosInterface>(std::move(impl));
}

PhobosInterface::~PhobosInterface() {}

void PhobosInterface::get_metadata(StorageObject& object)
{
    m_impl->get_metadata(object);
}

void PhobosInterface::put(const StorageObject& object, int fd)
{
    m_impl->put(object, fd);
}

void PhobosInterface::init()
{
    m_impl->init();
}

void PhobosInterface::finish()
{
    m_impl->finish();
}

std::string PhobosInterface::get(const StorageObject& object, int fd)
{
    return m_impl->get(object, fd);
}

bool PhobosInterface::exists(const StorageObject& obj)
{
    return m_impl->exists(obj);
}

void PhobosInterface::remove(const StorageObject& object)
{
    m_impl->remove(object);
}

void PhobosInterface::list(
    const KeyValuePair& query, std::vector<StorageObject>& found)
{
    m_impl->list(query, found);
}
}  // namespace hestia