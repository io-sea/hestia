#include "PhobosClient.h"

#include "FifoStreamSink.h"
#include "FifoStreamSource.h"
#include "ProjectConfig.h"

#include "IPhobosInterfaceImpl.h"

#include <stdexcept>

namespace hestia {

PhobosClient::PhobosClient(PhobosInterface::Ptr phobos_interface)
{
    if (phobos_interface) {
        m_phobos_interface = std::move(phobos_interface);
    }
    else {
        m_phobos_interface = PhobosInterface::create();
    }
}

std::unique_ptr<PhobosClient> PhobosClient::create(
    PhobosInterface::Ptr phobos_interface)
{
    return std::make_unique<PhobosClient>(std::move(phobos_interface));
}

PhobosClient::~PhobosClient() {}

std::string PhobosClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name() + "::PhobosClient";
}

void PhobosClient::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    if (stream != nullptr) {
        auto fifo     = FifoStreamSink::create();
        auto fifo_ptr = fifo.get();

        const auto fd = fifo->get_read_descriptor();
        stream->set_sink(std::move(fifo));

        auto read_op = [this, object, fd]() {
            m_phobos_interface->put(object, fd);
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, read_op));
    }
    else {
        m_phobos_interface->put(object, -1);
    }
}

void PhobosClient::get(
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    m_phobos_interface->get_metadata(object);

    if (stream != nullptr) {
        auto fifo     = FifoStreamSource::create();
        auto fifo_ptr = fifo.get();

        auto fd = fifo->get_write_descriptor();
        stream->set_source(std::move(fifo));

        auto write_op = [this, object, fd]() {
            m_phobos_interface->get(object, fd);
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, write_op));
    }
}

void PhobosClient::list(
    const Metadata::Query& query, std::vector<StorageObject>& found) const
{
    m_phobos_interface->list(query, found);
}

bool PhobosClient::exists(const StorageObject& object) const
{
    return m_phobos_interface->exists(object);
}

void PhobosClient::remove(const StorageObject& object) const
{
    m_phobos_interface->remove(object);
}
}  // namespace hestia
