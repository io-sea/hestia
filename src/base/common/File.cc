#include "File.h"

#include "FileUtils.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

namespace hestia {
File::File(const Path& path) : m_path(path) {}

File::~File()
{
    close();
}

void File::set_path(const Path& path)
{
    m_path = path;
}

OpStatus File::write(const char* data, std::size_t length)
{
    if (m_fd < 0) {
        if (const auto status = open_for_write(); !status.ok()) {
            return status;
        }
    }
    errno   = 0;
    auto rc = ::write(m_fd, data, length);
    if (rc < 0) {
        const std::string msg = strerror(errno);
        return {OpStatus::Status::ERROR, 0, "Failed to write: " + msg};
    }
    return {};
}

OpStatus File::write_lines(const std::vector<std::string>& lines)
{
    for (const auto& line : lines) {
        const auto status = write((line + "\n").c_str(), line.size() + 1);
        if (!status.ok()) {
            return status;
        }
    }
    return {};
}

std::pair<OpStatus, File::ReadState> File::read(char* data, std::size_t length)
{
    if (m_fd == -1) {
        if (const auto open_status = open_for_read(); !open_status.ok()) {
            return {open_status, {}};
        }
    }

    File::ReadState read_state;
    errno         = 0;
    const auto rc = ::read(m_fd, data, length);
    if (rc < 0) {
        const std::string msg = strerror(errno);
        return {
            {OpStatus::Status::ERROR, 0, "Error in stream during read: " + msg},
            {}};
    }
    read_state.m_size_read = static_cast<std::size_t>(rc);
    if (read_state.m_size_read < length) {
        read_state.m_finished = true;
    }
    return {{}, read_state};
}

OpStatus File::seek_to(std::size_t offset)
{
    if (m_fd == -1) {
        if (const auto open_status = open_for_read(); !open_status.ok()) {
            return open_status;
        }
    }
    errno         = 0;
    const auto rc = ::lseek(m_fd, offset, SEEK_SET);
    if (rc < 0) {
        const std::string msg = strerror(errno);
        return {
            OpStatus::Status::ERROR, 0, "Error in stream during read: " + msg};
    }
    return {};
}

OpStatus File::read(std::string& buffer)
{
    const std::size_t buffer_size{4096 * 10};
    char char_buffer[buffer_size];
    while (true) {
        const auto status = read(char_buffer, buffer_size);
        if (!status.first.ok()) {
            return status.first;
        }
        buffer += std::string(char_buffer, status.second.m_size_read);
        if (status.second.m_finished) {
            break;
        }
    }
    return {};
}

std::pair<OpStatus, File::ReadState> File::read_lines(
    std::vector<std::string>& lines)
{
    std::string content;
    const auto status = read(content);
    if (!status.ok()) {
        return {status, {}};
    }
    StringUtils::split(content, '\n', lines);

    File::ReadState read_state;
    read_state.m_finished  = true;
    read_state.m_size_read = content.size();
    return {{}, read_state};
}

OpStatus File::close()
{
    if (m_fd >= 0) {
        ::close(m_fd);
    }
    return {};
}

OpStatus File::open_for_write() noexcept
{
    if (m_path.empty()) {
        return {
            OpStatus::Status::ERROR, 0, "Tried to open file with empty path."};
    }

    try {
        FileUtils::create_if_not_existing(m_path);
    }
    catch (const std::exception& e) {
        return {
            OpStatus::Status::ERROR, 0,
            "Exception creating path: " + std::string(e.what()) + " at "
                + m_path.string()};
    }
    catch (...) {
        return {
            OpStatus::Status::ERROR, 0,
            "Unknown Exception creating path at " + m_path.string()};
    }

    errno = 0;
    m_fd =
        ::open(m_path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (m_fd < 0) {
        std::string msg = strerror(errno);
        return {
            OpStatus::Status::ERROR, 0,
            "Failed to open file for writing at: " + m_path.string() + " "
                + msg};
    }
    return {};
}

OpStatus File::open_for_read() noexcept
{
    if (m_path.empty()) {
        return {
            OpStatus::Status::ERROR, 0, "Tried to open file with empty path."};
    }

    if (!std::filesystem::is_regular_file(m_path)) {
        return {
            OpStatus::Status::ERROR, 0,
            "Couldn't open file at: " + m_path.string()};
    }

    errno = 0;
    m_fd  = ::open(m_path.c_str(), O_RDONLY);
    if (m_fd < 0) {
        std::string msg = strerror(errno);
        return {
            OpStatus::Status::ERROR, 0,
            "Failed to open file for read at: " + m_path.string() + " " + msg};
    }
    return {};
}

uintmax_t File::get_size() const
{
    return FileUtils::get_file_size(m_path);
}

}  // namespace hestia