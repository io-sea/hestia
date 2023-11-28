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

    ~SyncTool();

    void archive();

    void restore(const std::string& dataset_id);

  private:
    void take_snapshot(FilesystemSnapshot& snapshot);

    int archive(const FilesystemSnapshot& snapshot);

    std::string archive_file(const FilesystemEntry& fs_entry);

    void restore_object(const std::string& id, const std::string& path);

    FilesystemSnapshot m_current_snapshot;
    std::filesystem::path m_sync_dir;
};
}  // namespace hestia