#pragma once

#include <filesystem>
#include <vector>

namespace hestia {

struct FilesystemEntry {
    std::filesystem::path m_relative_path;
    std::time_t m_last_accessed_time{0};
    std::time_t m_last_modified_time{0};
    std::time_t m_created_time{0};
};

struct FilesystemSnapshot {
    std::vector<FilesystemEntry> m_files;
};

struct DatasetEntry {
    DatasetEntry() = default;

    DatasetEntry(const std::string& object_id, const std::string& path) :
        m_object_id(object_id), m_path(path)
    {
    }

    void from_string(const std::string& entry);

    std::string to_string() const;

    std::string m_object_id;
    std::string m_path;
    bool m_valid{true};
};

struct Dataset {
    void from_string(const std::string& payload);

    std::string to_string() const;

    std::vector<DatasetEntry> m_entries;
};

class SyncTool {
  public:
    SyncTool(
        const std::filesystem::path& sync_dir, const std::string& config_path);

    ~SyncTool();

    void archive(const std::string& sample_path, std::size_t sample_frequency);

    int sync(const std::string& dataset_id);

    int restore(const std::string& dataset_id);

  private:
    void take_snapshot(FilesystemSnapshot& snapshot);

    int get_dataset(const std::string& dataset_id, Dataset& dataset);

    int put_dataset(Dataset& dataset, const std::string& dataset_id = {});

    int archive(
        const FilesystemSnapshot& snapshot,
        const std::string& sample_path,
        std::size_t sample_frequency);

    std::string archive(const FilesystemEntry& fs_entry);

    int restore(const DatasetEntry& dataset_entry);

    void remove(const std::string& object_id);

    FilesystemSnapshot m_current_snapshot;
    std::filesystem::path m_sync_dir;
};
}  // namespace hestia