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

class SyncTool {
  public:
    SyncTool(const std::filesystem::path& sync_dir);

    void archive(const std::string& dataset_name);

    void restore(const std::string& dataset_name);

  private:
    void take_snapshot(FilesystemSnapshot& snapshot);

    void archive(
        const FilesystemSnapshot& snapshot, const std::string& dataset_name);

    std::string archive_file(const FilesystemEntry& fs_entry);

    void restore_object(const std::string& id);

    FilesystemSnapshot m_current_snapshot;
    std::filesystem::path m_sync_dir;
};
}  // namespace hestia