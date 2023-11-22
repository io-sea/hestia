#include "PhobosClient.h"

#include "FifoStreamSink.h"
#include "FifoStreamSource.h"
#include "ProjectConfig.h"

#include "IPhobosInterfaceImpl.h"

#include "Logger.h"

#include <stdexcept>

namespace hestia {

PhobosClient::PhobosClient(PhobosInterface::Ptr phobos_interface)
{
    LOG_INFO("Creating Phobos Client");
    if (phobos_interface) {
        m_phobos_interface = std::move(phobos_interface);
    }
    else {
        m_phobos_interface = PhobosInterface::create();
    }
    m_phobos_interface->init();
}

std::unique_ptr<PhobosClient> PhobosClient::create(
    PhobosInterface::Ptr phobos_interface)
{
    return std::make_unique<PhobosClient>(std::move(phobos_interface));
}

PhobosClient::~PhobosClient()
{
    m_phobos_interface->finish();
}

std::string PhobosClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name() + "::PhobosClient";
}

void PhobosClient::put(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc) const
{
    if (stream != nullptr) {
        auto fifo     = FifoStreamSink::create();
        auto fifo_ptr = fifo.get();

        const auto fd = fifo->get_read_descriptor();
        stream->set_sink(std::move(fifo));

        auto read_op = [this, request, fd, completion_func, id = m_id]() {
            auto response = ObjectStoreResponse::create(request, m_id);
            m_phobos_interface->put(request.object(), fd);
            completion_func(std::move(response));
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, read_op));
    }
    else {
        auto response = ObjectStoreResponse::create(request, m_id);
        m_phobos_interface->put(request.object(), -1);
        completion_func(std::move(response));
    }
}

void PhobosClient::get(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc) const
{
    StorageObject object = request.object();
    m_phobos_interface->get_metadata(object);

    if (stream != nullptr) {
        auto fifo     = FifoStreamSource::create();
        auto fifo_ptr = fifo.get();

        auto fd = fifo->get_write_descriptor();
        stream->set_source(std::move(fifo));

        auto write_op = [this, object, request, fd, completion_func,
                         id = m_id]() {
            auto response = ObjectStoreResponse::create(request, m_id);
            m_phobos_interface->get(object, fd);
            response->object() = object;
            completion_func(std::move(response));
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, write_op));
    }
    else {
        auto response      = ObjectStoreResponse::create(request, m_id);
        response->object() = object;
        completion_func(std::move(response));
    }
}

void PhobosClient::list(
    const KeyValuePair& query, std::vector<StorageObject>& found) const
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
