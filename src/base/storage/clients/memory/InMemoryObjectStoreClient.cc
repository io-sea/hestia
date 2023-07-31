#include "InMemoryObjectStoreClient.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

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
    StorageObject& object, const Extent& extent, Stream* stream) const
{
    auto md_iter = m_metadata.find(object.id());
    if (md_iter == m_metadata.end()) {
        const std::string msg =
            "Object " + object.id() + " not found in store.";
        LOG_ERROR(msg);
        throw ObjectStoreException(
            {ObjectStoreErrorCode::OBJECT_NOT_FOUND, msg});
    }
    object.get_metadata_as_writeable().merge(md_iter->second);

    if (stream != nullptr) {
        auto source_func =
            [this, object, extent](
                WriteableBufferView& buffer,
                std::size_t offset) -> InMemoryStreamSource::Status {
            (void)offset;
            const auto status = m_data.read(object.id(), extent, buffer);
            return {status.is_ok(), status.m_bytes_read};
        };
        auto source = InMemoryStreamSource::create(source_func);
        source->set_size(extent.m_length);
        stream->set_source(std::move(source));
    }
}

void InMemoryObjectStoreClient::put(
    const StorageObject& object, const Extent& extent, Stream* stream) const
{
    LOG_INFO("Starting client PUT: " + object.to_string());
    auto md_iter = m_metadata.find(object.id());
    if (md_iter == m_metadata.end()) {
        m_metadata[object.id()] = object.metadata();
    }
    else {
        md_iter->second.merge(object.metadata());
    }

    if (stream != nullptr) {
        auto sink_func = [this, object, extent](
                             const ReadableBufferView& buffer,
                             std::size_t offset) -> InMemoryStreamSink::Status {
            const Extent chunk_extent = {
                extent.m_offset + offset, buffer.length()};
            const auto status = m_data.write(object.id(), chunk_extent, buffer);
            return {status.is_ok(), buffer.length()};
        };
        auto sink = InMemoryStreamSink::create(sink_func);
        sink->set_size(extent.m_length);
        stream->set_sink(std::move(sink));
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