#include "SyncTool.h"

#include "File.h"
#include "IdGenerator.h"
#include "Logger.h"
#include "StringUtils.h"
#include "TimeUtils.h"

#include "hestia_iosea.h"

#include <iostream>

namespace hestia {

void DatasetEntry::from_string(const std::string& entry)
{
    std::tie(m_object_id, m_path) = StringUtils::split_on_first(entry, ",");
    if (m_object_id.size() != 36) {
        m_valid = false;
    }
}

std::string DatasetEntry::to_string() const
{
    return m_object_id + "," + m_path;
}

void Dataset::from_string(const std::string& payload)
{
    std::vector<std::string> lines;
    StringUtils::to_lines(payload, lines);
    for (const auto& line : lines) {
        DatasetEntry entry;
        entry.from_string(line);
        if (entry.m_valid) {
            m_entries.emplace_back().from_string(line);
        }
    }
}

std::string Dataset::to_string() const
{
    std::string payload;
    for (const auto& entry : m_entries) {
        payload += entry.to_string() + "\n";
    }
    return payload;
}

SyncTool::SyncTool(
    const std::filesystem::path& sync_dir, const std::string& config_path) :
    m_sync_dir(sync_dir)
{
    if (config_path.empty()) {
        hestia_initialize(nullptr, nullptr, nullptr);
    }
    else {
        hestia_initialize(config_path.c_str(), nullptr, nullptr);
    }
    hestia_output_info();
}

SyncTool::~SyncTool()
{
    hestia_finish();
}

void SyncTool::archive(
    const std::string& sample_path, std::size_t sample_frequency)
{
    take_snapshot(m_current_snapshot);

    archive(m_current_snapshot, sample_path, sample_frequency);
}

int SyncTool::put_dataset(Dataset& dataset, const std::string& dataset_id)
{
    const auto payload_str = dataset.to_string();
    std::vector<char> payload(payload_str.begin(), payload_str.end());
    payload.push_back('\0');

    bool is_update = !dataset_id.empty();

    HestiaId id;
    if (is_update) {
        hestia_init_id(&id);
        StringUtils::to_char(dataset_id, &id.m_uuid);
    }

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload.data();
    io_ctx.m_length = payload.size();

    HestiaTier tier;

    auto insert_mode = is_update ? hestia_create_mode_t::HESTIA_UPDATE :
                                   hestia_create_mode_t::HESTIA_CREATE;
    auto rc          = hestia_object_put(&id, insert_mode, &io_ctx, &tier);
    if (rc != 0) {
        std::cerr << "Dataset put failed" << std::endl;
        return rc;
    }

    std::cout << "archived dataset to object: " << id.m_uuid << std::endl;
    if (is_update) {
        delete[] id.m_uuid;
    }
    else {
        std::string new_id_buf(id.m_uuid);
        HestiaId new_id;
        hestia_init_id(&new_id);
        StringUtils::to_char(new_id_buf, &new_id.m_uuid);

        HestiaKeyValuePair kv_pairs[1];
        StringUtils::to_char("is_dataset", &kv_pairs[0].m_key);
        StringUtils::to_char("true", &kv_pairs[0].m_value);
        hestia_object_set_attrs(&new_id, kv_pairs, 1);
        delete[] kv_pairs[0].m_key;
        delete[] kv_pairs[0].m_value;
        delete[] new_id.m_uuid;
        hestia_free_id(&id);
    }
    return 0;
}

int SyncTool::archive(
    const FilesystemSnapshot& snapshot,
    const std::string& sample_path,
    std::size_t sample_frequency)
{
    Dataset dataset;
    std::size_t count{0};
    std::size_t total = snapshot.m_files.size();

    File sample_file;
    bool sampling_active = !sample_path.empty();
    if (sampling_active) {
        sample_file.set_path(sample_path);
    }

    const auto initial_time = TimeUtils::get_time_since_epoch_micros();
    for (const auto& fs_entry : snapshot.m_files) {
        const auto id = archive(fs_entry);
        if (id.empty()) {
            return -1;
        }
        dataset.m_entries.emplace_back(id, fs_entry.m_relative_path);
        if (count % sample_frequency == 0) {

            std::cout << count << " of " << total << std::endl;
            if (sampling_active) {
                auto time_delta = std::to_string(
                    (TimeUtils::get_time_since_epoch_micros() - initial_time)
                        .count());
                time_delta += '\n';
                sample_file.write(time_delta.c_str(), time_delta.size());
            }
        }
        count++;
    }
    return put_dataset(dataset);
}

std::string SyncTool::archive(const FilesystemEntry& fs_entry)
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
        char error_buf[1024];
        hestia_get_last_error(error_buf, 1024);
        std::cerr << "Object put failed: " << std::string(error_buf)
                  << std::endl;
        return {};
    }

    HestiaKeyValuePair kv_pairs[1];
    StringUtils::to_char("filesystem_path", &kv_pairs[0].m_key);
    StringUtils::to_char(
        fs_entry.m_relative_path.string(), &kv_pairs[0].m_value);
    hestia_object_set_attrs(&id, kv_pairs, 1);

    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
    delete[] kv_pairs[0].m_key;
    delete[] kv_pairs[0].m_value;
    return object_id;
}

int SyncTool::get_dataset(const std::string& dataset_id, Dataset& dataset)
{
    HestiaId id;
    hestia_init_id(&id);

    StringUtils::to_char(dataset_id, &id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&id, &object);
    if (rc != 0) {
        std::cerr << "Error getting object attributes: " + dataset_id
                  << std::endl;
        return rc;
    }

    std::vector<char> payload_buffer(object.m_size);

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type   = hestia_io_type_t::HESTIA_IO_BUFFER;
    io_ctx.m_buffer = payload_buffer.data();
    io_ctx.m_length = payload_buffer.size();

    HestiaTier tier;

    rc = hestia_object_get(&id, &io_ctx, &tier);
    if (rc != 0) {
        std::cerr << "Error getting object data: " + dataset_id << std::endl;
        return rc;
    }

    hestia_init_object(&object);
    delete[] id.m_uuid;

    dataset.from_string({payload_buffer.begin(), payload_buffer.end()});
    return 0;
}

int SyncTool::restore(const std::string& dataset_id)
{
    Dataset dataset;
    auto rc = get_dataset(dataset_id, dataset);
    if (rc != 0) {
        std::cerr << "Error getting dataset: " << dataset_id << std::endl;
        return rc;
    }

    for (const auto& entry : dataset.m_entries) {
        rc = restore(entry);
        if (rc != 0) {
            std::cerr << "Error getting dataset entry for dataset: "
                      << dataset_id << std::endl;
            return rc;
        }
    }
    return 0;
}

int SyncTool::sync(const std::string& dataset_id)
{
    FilesystemSnapshot snapshot;
    take_snapshot(snapshot);

    Dataset dataset;
    auto rc = get_dataset(dataset_id, dataset);
    if (rc != 0) {
        return rc;
    }

    std::vector<std::string> for_removal;
    Dataset updated_dataset;
    for (const auto& entry : dataset.m_entries) {
        bool found = false;
        for (const auto& fs_entry : snapshot.m_files) {
            if (entry.m_path == fs_entry.m_relative_path.string()) {
                found = true;
                break;
            }
        }
        if (!found) {
            for_removal.push_back(entry.m_object_id);
        }
        else {
            updated_dataset.m_entries.push_back(entry);
        }
    }

    bool has_new_entries{false};
    for (const auto& fs_entry : snapshot.m_files) {
        bool found = false;
        for (const auto& entry : dataset.m_entries) {
            if (entry.m_path == fs_entry.m_relative_path.string()) {
                found = true;
                break;
            }
        }
        if (!found) {
            const auto id = archive(fs_entry);
            updated_dataset.m_entries.emplace_back(
                id, fs_entry.m_relative_path);
            has_new_entries = true;
        }
    }

    if (has_new_entries || !for_removal.empty()) {
        rc = put_dataset(updated_dataset, dataset_id);
        if (rc != 0) {
            return rc;
        }

        for (const auto& object_id : for_removal) {
            remove(object_id);
        }
    }
    return 0;
}

void SyncTool::remove(const std::string& object_id)
{
    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(object_id, &id.m_uuid);

    // First release data from tiers
    uint8_t* tier_ids{nullptr};
    std::size_t num_tier_ids{0};
    hestia_object_locate(&id, &tier_ids, &num_tier_ids);
    for (std::size_t idx = 0; idx < num_tier_ids; idx++) {
        hestia_object_release(&id, tier_ids[idx], 0);
    }
    hestia_free_tier_ids(&tier_ids);

    // Then remove the object
    hestia_object_remove(&id);

    delete[] id.m_uuid;
}

int SyncTool::restore(const DatasetEntry& dataset_entry)
{
    std::cout << "Restoring id: " + dataset_entry.m_object_id << std::endl;
    HestiaId id;
    hestia_init_id(&id);
    StringUtils::to_char(dataset_entry.m_object_id, &id.m_uuid);

    HestiaObject object;
    auto rc = hestia_object_get_attrs(&id, &object);
    if (rc != 0) {
        std::cerr << "Error getting object attrs for id: "
                  << dataset_entry.m_object_id << std::endl;
        return rc;
    }

    HestiaIoContext io_ctx;
    hestia_init_io_ctx(&io_ctx);
    io_ctx.m_type = hestia_io_type_t::HESTIA_IO_PATH;

    std::string absolute_path =
        (m_sync_dir / std::filesystem::path(dataset_entry.m_path)).string();
    StringUtils::to_char(absolute_path, &io_ctx.m_path);

    HestiaTier tier;
    rc = hestia_object_get(&id, &io_ctx, &tier);
    if (rc != 0) {
        std::cerr << "Error getting object data" << std::endl;
        return rc;
    }

    hestia_init_object(&object);
    delete[] id.m_uuid;
    delete[] io_ctx.m_path;
    std::cout << "Finished Restoring id: " + dataset_entry.m_object_id
              << std::endl;
    return 0;
}

void SyncTool::take_snapshot(FilesystemSnapshot& snapshot)
{
    for (const auto& p :
         std::filesystem::recursive_directory_iterator(m_sync_dir)) {
        if (!std::filesystem::is_directory(p)) {
            FilesystemEntry fs_entry;
            fs_entry.m_relative_path =
                std::filesystem::relative(p.path(), m_sync_dir);
            snapshot.m_files.push_back(fs_entry);
        }
    }
}
}  // namespace hestia