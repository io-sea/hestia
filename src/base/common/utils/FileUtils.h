#pragma once

#include <filesystem>

namespace hestia {

/**
 * @brief Helper methods for working with files.
 *
 * Helper methods for working with files. There is a File class which
 * may suit better for others.
 */
class FileUtils {
  public:
    using Path = std::filesystem::path;

    /**
     * Create this path if it doesn't already exist. No-op if it exists.
     *
     * @param path the path to create
     */
    static void create_if_not_existing(const Path& path);

    /**
     * True if this directory entry is a file with specified extension
     *
     * @param dir_entry the directory entry to check
     * @param extension the extension it should have (e.g. '.txt')
     * @return True if this directory entry is a file with specified extension
     */
    static bool is_file_with_extension(
        const std::filesystem::directory_entry& dir_entry,
        const std::string& extension);

    /**
     * Get the name of a file in this path without the extension (i.e.
     * /path/to/file.txt becomes 'file')
     *
     * @param path the path to the file
     * @return the filename only
     */
    static std::string get_filename_without_extension(const Path& path);

    static void empty_path(const Path& path);

    /**
     * Get a file's size in bytes.
     *
     * @param path the file path
     */
    static uintmax_t get_file_size(const Path& path);
};
}  // namespace hestia