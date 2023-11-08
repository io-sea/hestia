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
    return m_metadata.find(object.id()) != m_metadata.end();
}

std::string InMemoryObjectStoreClient::dump() const
{
    return m_data.dump();
}

void InMemoryObjectStoreClient::migrate(
    const std::string& object_id,
    InMemoryObjectStoreClient* target_client,
    bool delete_after)
{
    auto iter                            = m_metadata.find(object_id);
    target_client->m_metadata[object_id] = iter->second;

    if (delete_after) {
        m_metadata.erase(object_id);
    }

    target_client->m_data.set_block_list(
        object_id, m_data.get_block_list(object_id));
    if (delete_after) {
        m_data.remove(object_id);
    }
}

void InMemoryObjectStoreClient::get(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc progress_func) const
{
    auto md_iter = m_metadata.find(request.object().id());
    if (md_iter == m_metadata.end()) {
        const std::string msg = SOURCE_LOC() + " | Object "
                                + request.object().id()
                                + " not found in store.";
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
    }

    auto response = ObjectStoreResponse::create(request, m_id);
    response->object().merge_metadata(md_iter->second);

    if (stream == nullptr) {
        completion_func(std::move(response));
    }
    else {
        auto source_func = [this, object_id = request.object().id(),
                            extent = request.extent()](
                               WriteableBufferView& buffer,
                               std::size_t) -> InMemoryStreamSource::Status {
            const auto status = m_data.read(object_id, extent, buffer);
            return {status.is_ok(), status.m_bytes_read};
        };

        auto source = InMemoryStreamSource::create(source_func);
        source->set_size(request.extent().m_length);
        stream->set_source(std::move(source));
        auto stream_completion_func = [completion_func,
                                       object = response->object(), request,
                                       id     = m_id](StreamState state) {
            auto response      = ObjectStoreResponse::create(request, id);
            response->object() = object;
            if (!state.ok()) {
                response->on_error(
                    {ObjectStoreErrorCode::BAD_STREAM, state.message()});
            }
            completion_func(std::move(response));
        };
        stream->set_completion_func(std::move(stream_completion_func));
        stream->set_progress_func(
            request.get_progress_interval(), progress_func);
    }
}

void InMemoryObjectStoreClient::put(
    const ObjectStoreRequest& request,
    completionFunc completion_func,
    Stream* stream,
    Stream::progressFunc progress_func) const
{
    LOG_INFO("Starting client PUT: " + request.object().to_string());
    auto md_iter = m_metadata.find(request.object().id());
    if (md_iter == m_metadata.end()) {
        m_metadata[request.object().id()] = request.object().metadata();
    }
    else {
        md_iter->second.merge(request.object().metadata());
    }

    if (stream == nullptr) {
        completion_func(ObjectStoreResponse::create(request, m_id));
    }

    else {
        auto sink_func = [this, request](
                             const ReadableBufferView& buffer,
                             std::size_t offset) -> InMemoryStreamSink::Status {
            const Extent chunk_extent = {
                request.extent().m_offset + offset, buffer.length()};
            const auto status =
                m_data.write(request.object().id(), chunk_extent, buffer);
            return {status.is_ok(), buffer.length()};
        };

        auto stream_completion_func = [completion_func, request,
                                       id = m_id](StreamState state) {
            auto response = ObjectStoreResponse::create(request, id);
            if (!state.ok()) {
                response->on_error(
                    {ObjectStoreErrorCode::BAD_STREAM, state.message()});
            }
            completion_func(std::move(response));
        };

        auto sink = InMemoryStreamSink::create(sink_func);
        sink->set_size(request.extent().m_length);
        stream->set_sink(std::move(sink));
        stream->set_completion_func(stream_completion_func);
        stream->set_progress_func(
            request.get_progress_interval(), progress_func);
    }
}

void InMemoryObjectStoreClient::remove(const StorageObject& object) const
{
    const auto obj_id = object.id();
    auto md_iter      = m_metadata.find(obj_id);
    if (md_iter == m_metadata.end()) {
        const std::string msg = "Object " + obj_id + " not found in store.";
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
    }
    m_metadata.erase(md_iter);
    if (m_data.has_key(obj_id)) {
        m_data.remove(obj_id);
    }
}

void InMemoryObjectStoreClient::list(
    const KeyValuePair& query,
    std::vector<StorageObject>& matching_objects) const
{
    for (const auto& [key, md] : m_metadata) {
        if (md.has_key_and_value(query)) {
            StorageObject object(key);
            object.get_metadata_as_writeable() = md;
            matching_objects.push_back(object);
        }
    }
}
}  // namespace hestia