#include "InMemoryObjectStoreClient.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "ErrorUtils.h"
#include "Logger.h"
#include "ProjectConfig.h"

#include <iostream>

namespace hestia {
InMemoryObjectStoreClient::Ptr InMemoryObjectStoreClient::create()
{
    return std::make_unique<InMemoryObjectStoreClient>();
}

std::string InMemoryObjectStoreClient::get_registry_identifier()
{
    return hestia::project_config::get_project_name()
           + "::InMemoryObjectStoreClient";
}

bool InMemoryObjectStoreClient::exists(const StorageObject& object) const
{
    std::scoped_lock lck(m_metadata_mutex);
    return m_metadata.find(object.id()) != m_metadata.end();
}

std::string InMemoryObjectStoreClient::dump() const
{
    std::scoped_lock lck(m_data_mutex);
    return m_data.dump();
}

void InMemoryObjectStoreClient::upsert_metadata(
    const StorageObject& object) const
{
    std::scoped_lock lck(m_metadata_mutex);
    if (auto md_iter = m_metadata.find(object.id());
        md_iter == m_metadata.end()) {
        m_metadata[object.id()] = object.metadata();
    }
    else {
        md_iter->second.merge(object.metadata());
    }
}

void InMemoryObjectStoreClient::read_metadata(StorageObject& object) const
{
    std::scoped_lock lck(m_metadata_mutex);
    auto md_iter = m_metadata.find(object.id());
    if (md_iter == m_metadata.end()) {
        const std::string msg =
            "Object " + object.id() + " not found in store.";
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND,
             SOURCE_LOC() + " | " + msg});
    }
    object.merge_metadata(md_iter->second);
}

void InMemoryObjectStoreClient::migrate(
    const std::string& object_id,
    InMemoryObjectStoreClient* target_client,
    bool delete_after)
{
    {
        std::scoped_lock lck(m_metadata_mutex);
        auto iter = m_metadata.find(object_id);
        if (iter == m_metadata.end()) {
            on_object_not_found(SOURCE_LOC(), object_id);
        }
        target_client->m_metadata[object_id] = iter->second;

        if (delete_after) {
            m_metadata.erase(object_id);
        }
    }

    {
        std::scoped_lock lck(m_data_mutex);
        target_client->m_data.set_block_list(
            object_id, m_data.get_block_list(object_id));
        if (delete_after) {
            m_data.remove(object_id);
        }
    }
}

void InMemoryObjectStoreClient::get(ObjectStoreContext& ctx) const
{
    const auto object_id = ctx.m_request.object().id();
    if (!exists(ctx.m_request.object())) {
        on_object_not_found(SOURCE_LOC(), object_id);
    }

    auto response = ObjectStoreResponse::create(ctx.m_request, m_id);
    read_metadata(response->object());

    if (ctx.has_stream()) {
        auto source_func = [this, object_id, extent = ctx.m_request.extent()](
                               WriteableBufferView& buffer,
                               std::size_t) -> InMemoryStreamSource::Status {
            BlockStore::ReturnCode status;
            {
                std::scoped_lock lck(m_data_mutex);
                status = m_data.read(object_id, extent, buffer);
            }
            return {status.is_ok(), status.m_bytes_read};
        };

        auto source = InMemoryStreamSource::create(source_func);
        source->set_size(ctx.m_request.extent().m_length);
        ctx.m_stream->set_source(std::move(source));
        init_stream(ctx, response->object());
    }
    else {
        ctx.m_completion_func(std::move(response));
    }
}

void InMemoryObjectStoreClient::put(ObjectStoreContext& ctx) const
{
    const auto object_id = ctx.m_request.object().id();

    upsert_metadata(ctx.m_request.object());

    if (ctx.has_stream()) {
        auto sink_func =
            [this, object_id, extent = ctx.m_request.extent()](
                const ReadableBufferView& buffer,
                std::size_t offset) -> InMemoryStreamSource::Status {
            const Extent chunk_extent = {
                extent.m_offset + offset, buffer.length()};
            BlockStore::ReturnCode status;
            {
                std::scoped_lock lck(m_data_mutex);
                status = m_data.write(object_id, chunk_extent, buffer);
            }
            return {status.is_ok(), buffer.length()};
        };
        auto sink = InMemoryStreamSink::create(sink_func);
        sink->set_size(ctx.m_request.extent().m_length);
        ctx.m_stream->set_sink(std::move(sink));
        init_stream(ctx);
    }
    else {
        on_success(ctx);
    }
}

void InMemoryObjectStoreClient::remove(const StorageObject& object) const
{
    const auto obj_id = object.id();
    {
        std::scoped_lock lck(m_metadata_mutex);
        auto md_iter = m_metadata.find(obj_id);
        if (md_iter == m_metadata.end()) {
            on_object_not_found(SOURCE_LOC(), obj_id);
        }
        m_metadata.erase(md_iter);
    }

    {
        std::scoped_lock lck(m_data_mutex);
        if (m_data.has_key(obj_id)) {
            m_data.remove(obj_id);
        }
    }
}

void InMemoryObjectStoreClient::list(
    const KeyValuePair& query,
    std::vector<StorageObject>& matching_objects) const
{
    std::scoped_lock lck(m_metadata_mutex);
    for (const auto& [key, md] : m_metadata) {
        if (md.has_key_and_value(query)) {
            StorageObject object(key);
            object.get_metadata_as_writeable() = md;
            matching_objects.push_back(object);
        }
    }
}
}  // namespace hestia