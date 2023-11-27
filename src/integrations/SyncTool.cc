#include "SyncTool.h"

#include "IdGenerator.h"
#include "Logger.h"
#include "StringUtils.h"

#include "hestia_iosea.h"

namespace hestia {
SyncTool::SyncTool(const std::filesystem::path& sync_dir) : m_sync_dir(sync_dir)
{
}

void SyncTool::archive(const std::string& dataset_name)
{
    hestia_initialize(nullptr, nullptr, nullptr);

    take_snapshot(m_current_snapshot);

    archive(m_current_snapshot, dataset_name);

    hestia_finish();
}

void SyncTool::archive(
    const FilesystemSnapshot& snapshot, const std::string& dataset_name)
{
    std::vector<std::string> ids;
    for (const auto& fs_entry : snapshot.m_files) {
        const auto id = archive_file(fs_entry);
        ids.push_back(id);
    }

    // Add the dataset object
    const auto payload_str = StringUtils::flatten(ids);
    std::vector<char> payload(payload_str.begin(), payload_str.end());
    payload.push_back('\0');

    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(dataset_name, &id.m_name);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    auto rc =
        hestia_object_put(&id, hestia_create_mode_t::HESTIA_CREATE, &io_ctx, 0);
    if (rc != 0) {
        LOG_ERROR("Object put failed");
        return;
    }
    delete[] id.m_name;
}

std::string SyncTool::archive_file(const FilesystemEntry& fs_entry)
{
    HestiaId id;
    hestia_init_id(&id);
    const auto relative_path = fs_entry.m_relative_path.string();
    const auto object_id     = DefaultIdGenerator().get_id(relative_path);
    StringUtils::to_char(object_id, &id.m_uuid);

    const auto absolute_path = (m_sync_dir / fs_entry.m_relative_path).string();
    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    StringUtils::to_char(absolute_path, &io_ctx.m_path);
    io_ctx.m_type = hestia_io_type_t::HESTIA_IO_PATH;

    auto rc =
        hestia_object_put(&id, hestia_create_mode_t::HESTIA_CREATE, &io_ctx, 0);
    if (rc != 0) {
        LOG_ERROR("Object put failed");
        return {};
    }

    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
    return object_id;
}

void SyncTool::restore(const std::string& dataset_name)
{
    hestia_initialize(nullptr, nullptr, nullptr);

    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(dataset_name, &id.m_name);

    HestiaObject object;
    hestia_object_get_attrs(&id, &object);

    std::vector<char> payload(object.m_size);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    hestia_object_get(&id, &io_ctx, 0);

    std::vector<std::string> object_ids;
    StringUtils::split(
        std::string(payload.begin(), payload.end()), ",", object_ids);

    hestia_init_object(&object);
    delete[] id.m_name;

    for (const auto& id : object_ids) {
        restore_object(id);
    }

    hestia_finish();
}

void SyncTool::restore_object(const std::string& uuid)
{
    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(uuid, &id.m_uuid);

    HestiaObject object;
    hestia_object_get_attrs(&id, &object);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type = hestia_io_type_t::HESTIA_IO_PATH;

    std::string absolute_path =
        (m_sync_dir / std::filesystem::path(object.m_name)).string();
    StringUtils::to_char(absolute_path, &io_ctx.m_path);

    hestia_object_get(&id, &io_ctx, 0);

    hestia_init_object(&object);
    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
}

void SyncTool::take_snapshot(FilesystemSnapshot& snapshot)
{
    for (const auto& p :
         std::filesystem::recursive_directory_iterator(m_sync_dir)) {
        if (!std::filesystem::is_directory(p)) {
            FilesystemEntry fs_entry;
            fs_entry.m_relative_path =
                std::filesystem::relative(p.path(), m_sync_dir);

            LOG_INFO("Got path: " << fs_entry.m_relative_path);
            snapshot.m_files.push_back(fs_entry);
        }
    }
}
}  // namespace hestia