#include "FileUtils.h"

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
        if (!std::filesystem::is_directory(path.parent_path())) {
            std::filesystem::create_directories(path.parent_path());
        }
    }
}
}  // namespace hestia