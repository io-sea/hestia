#include "FileUtils.h"
#include <iostream>

namespace hestia {
bool FileUtils::is_file_with_extension(
    const std::filesystem::directory_entry& dir_entry,
    const std::string& extension)
{
    return dir_entry.is_regular_file()
           && dir_entry.path().extension().string() == extension;
}

std::string FileUtils::get_filename_without_extension(const Path& path)
{
    return path.stem().string();
}

void FileUtils::create_if_not_existing(const Path& path)
{
    if (!std::filesystem::is_regular_file(path)) {
        if (path.has_parent_path()
            && !std::filesystem::is_directory(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }
    }
}

void FileUtils::empty_directory(const Path& path)
{
    if (!std::filesystem::is_regular_file(path)) {
        std::filesystem::remove_all(path);
        std::filesystem::create_directories(path);
    }
}

uintmax_t FileUtils::get_file_size(const Path& path)
{
    return std::filesystem::file_size(path);
}

}  // namespace hestia