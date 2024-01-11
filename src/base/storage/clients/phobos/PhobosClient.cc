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

void PhobosClient::put(ObjectStoreContext& ctx) const
{
    if (ctx.has_stream()) {
        auto fifo     = FifoStreamSink::create();
        auto fifo_ptr = fifo.get();

        const auto fd = fifo->get_read_descriptor();
        ctx.m_stream->set_sink(std::move(fifo));

        auto read_op = [this, ctx, fd]() {
            auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
            try {
                m_phobos_interface->put(ctx.m_request.object(), fd);
            }
            catch (const std::exception& e) {
                response->on_error(
                    {ObjectStoreErrorCode::STL_EXCEPTION, e.what()});
                ctx.finish(std::move(response));
                return 0;
            }
            ctx.finish(std::move(response));
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, read_op));
    }
    else {
        auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
        m_phobos_interface->put(ctx.m_request.object(), -1);
        ctx.finish(std::move(response));
    }
}

void PhobosClient::get(ObjectStoreContext& ctx) const
{
    StorageObject object = ctx.m_request.object();
    m_phobos_interface->get_metadata(object);

    if (ctx.has_stream()) {
        auto fifo     = FifoStreamSource::create();
        auto fifo_ptr = fifo.get();

        auto fd = fifo->get_write_descriptor();
        ctx.m_stream->set_source(std::move(fifo));

        auto write_op = [this, ctx, fd, object]() {
            auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
            response->object() = object;
            try {
                const auto redirect_location =
                    m_phobos_interface->get(response->object(), fd);
                response->object().set_location(redirect_location);
            }
            catch (const std::exception& e) {
                response->on_error(
                    {ObjectStoreErrorCode::STL_EXCEPTION, e.what()});
                ctx.finish(std::move(response));
                return 0;
            }
            ctx.finish(std::move(response));
            return 0;
        };
        fifo_ptr->set_producer(std::async(std::launch::async, write_op));
    }
    else {
        auto response      = ObjectStoreResponse::create(ctx.m_request, m_id);
        response->object() = object;
        ctx.finish(std::move(response));
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
