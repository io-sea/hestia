#include "SyncTool.h"

#include "IdGenerator.h"
#include "Logger.h"
#include "StringUtils.h"

#include "hestia_iosea.h"

#include <iostream>

namespace hestia {
SyncTool::SyncTool(const std::filesystem::path& sync_dir) : m_sync_dir(sync_dir)
{
    hestia_initialize(nullptr, nullptr, nullptr);
}

SyncTool::~SyncTool()
{
    hestia_finish();
}

void SyncTool::archive()
{
    take_snapshot(m_current_snapshot);

    archive(m_current_snapshot);
}

int SyncTool::archive(const FilesystemSnapshot& snapshot)
{
    std::vector<std::pair<std::string, std::string>> paths_and_ids;
    for (const auto& fs_entry : snapshot.m_files) {
        const auto id = archive_file(fs_entry);
        if (id.empty()) {
            return -1;
        }
        paths_and_ids.emplace_back(id, fs_entry.m_relative_path);
    }

    // Add the dataset object
    std::string payload_str;
    for (const auto& [id, path] : paths_and_ids) {
        payload_str += id + "," + path + "\n";
    }
    std::cout << "payload is: " << payload_str << std::endl;
    std::vector<char> payload(payload_str.begin(), payload_str.end());
    payload.push_back('\0');

    HestiaId id;

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    HestiaTier tier;

    auto rc = hestia_object_put(
        &id, hestia_create_mode_t::HESTIA_CREATE, &io_ctx, &tier);
    if (rc != 0) {
        LOG_ERROR("Object put failed");
        return rc;
    }

    std::cout << "archived dataset to object: " << id.m_uuid << std::endl;
    hestia_free_id(&id);
    return 0;
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

    HestiaTier tier;

    auto rc = hestia_object_put(
        &id, hestia_create_mode_t::HESTIA_CREATE, &io_ctx, &tier);
    if (rc != 0) {
        LOG_ERROR("Object put failed");
        return {};
    }

    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
    return object_id;
}

void SyncTool::restore(const std::string& dataset_id)
{
    HestiaId id;
    hestia_init_id(&id);

    StringUtils::to_char(dataset_id, &id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&id, &object);
    if (rc != 0) {
        LOG_ERROR("Error getting object attributes: " + dataset_id);
        return;
    }

    std::vector<char> payload(object.m_size);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    HestiaTier tier;

    hestia_object_get(&id, &io_ctx, &tier);

    hestia_init_object(&object);
    delete[] id.m_uuid;

    std::string payload_str(payload.begin(), payload.end());
    std::cout << "payload is: " << payload_str << std::endl;

    std::vector<std::string> lines;
    StringUtils::to_lines(payload_str, lines);
    for (const auto& line : lines) {
        const auto& [id, path] = StringUtils::split_on_first(line, ",");
        if (id.size() < 36) {
            continue;
        }
        restore_object(id, path);
    }
}

void SyncTool::restore_object(const std::string& uuid, const std::string& path)
{
    LOG_INFO("Restoring id: " + uuid);
    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(uuid, &id.m_uuid);

    HestiaObject object;
    hestia_object_get_attrs(&id, &object);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type = hestia_io_type_t::HESTIA_IO_PATH;

    std::string absolute_path =
        (m_sync_dir / std::filesystem::path(path)).string();
    StringUtils::to_char(absolute_path, &io_ctx.m_path);

    HestiaTier tier;
    hestia_object_get(&id, &io_ctx, &tier);

    hestia_init_object(&object);
    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
    LOG_INFO("Finished Restoring id: " + uuid);
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