#include "File.h"

#include "FileUtils.h"

namespace hestia {
File::File(const Path& path) : m_path(path) {}

File::~File()
{
    close();
}

OpStatus File::write(const char* data, std::size_t length)
{
    if (!m_out_stream.is_open()) {
        if (const auto status = open_for_write(); !status.ok()) {
            return status;
        }
    }

    if (m_out_stream.bad()) {
        return {
            OpStatus::Status::ERROR, 0,
            "Out stream in bad state before write."};
    }

    m_out_stream.write(data, length);
    if (m_out_stream.bad()) {
        return {
            OpStatus::Status::ERROR, 0, "Out stream in bad state after write."};
    }
    return {};
}

OpStatus File::write_lines(const std::vector<std::string>& lines)
{
    if (!m_out_stream.is_open()) {
        if (const auto status = open_for_write(); !status.ok()) {
            return status;
        }
    }

    if (m_out_stream.bad()) {
        return {
            OpStatus::Status::ERROR, 0,
            "Out stream in bad state before write."};
    }

    for (const auto& line : lines) {
        m_out_stream << line << "\n";
    }

    if (m_out_stream.bad()) {
        return {
            OpStatus::Status::ERROR, 0, "Out stream in bad state after write."};
    }
    return {};
}

std::pair<OpStatus, File::ReadState> File::read(char* data, std::size_t length)
{
    if (!m_in_stream.is_open()) {
        if (const auto status = open_for_read(); !status.ok()) {
            return {{OpStatus::Status::ERROR, 0, status.message()}, {}};
        }
    }

    if (m_in_stream.bad()) {
        return {
            {OpStatus::Status::ERROR, 0, "In stream in bad state before read."},
            {}};
    }

    File::ReadState read_state;
    m_in_stream.read(data, length);
    if (m_in_stream.eof()) {
        read_state.m_finished = true;
    }
    read_state.m_size_read = static_cast<std::size_t>(m_in_stream.gcount());
    return {{}, read_state};
}

OpStatus File::read(std::string& buffer)
{
    if (!m_in_stream.is_open()) {
        if (const auto status = open_for_read(); !status.ok()) {
            return {OpStatus::Status::ERROR, 0, status.message()};
        }
    }

    if (m_in_stream.bad()) {
        return {
            OpStatus::Status::ERROR, 0, "In stream in bad state before read."};
    }

    buffer.assign(
        (std::istreambuf_iterator<char>(m_in_stream)),
        (std::istreambuf_iterator<char>()));

    return {};
}

std::pair<OpStatus, File::ReadState> File::read_lines(
    std::vector<std::string>& lines)
{
    if (!m_in_stream.is_open()) {
        if (const auto status = open_for_read(); !status.ok()) {
            return {{OpStatus::Status::ERROR, 0, status.message()}, {}};
        }
    }

    if (m_in_stream.bad()) {
        return {
            {OpStatus::Status::ERROR, 0,
             "In stream in bad state before write."},
            {}};
    }

    File::ReadState read_state;
    std::string line;
    while (std::getline(m_in_stream, line)) {
        lines.push_back(line);
    }

    read_state.m_finished  = true;
    read_state.m_size_read = static_cast<std::size_t>(m_in_stream.gcount());
    return {{}, read_state};
}

OpStatus File::close()
{
    if (m_out_stream) {
        m_out_stream.close();
        if (m_out_stream.bad()) {
            return {
                OpStatus::Status::ERROR, 0,
                "Out stream in bad state when closing file."};
        }
    }
    if (m_in_stream) {
        m_in_stream.close();
        if (m_in_stream.bad()) {
            return {
                OpStatus::Status::ERROR, 0,
                "In stream in bad state when closing file."};
        }
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

    m_out_stream = std::ofstream(m_path);
    if (!m_out_stream.good()) {
        return {
            OpStatus::Status::ERROR, 0,
            "Failed to create valid output stream at " + m_path.string()};
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

    m_in_stream = std::ifstream(m_path);
    if (!m_in_stream.good()) {
        return {
            OpStatus::Status::ERROR, 0,
            "Failed to create valid input stream at: " + m_path.string()};
    }
    return {};
}

uintmax_t File::get_size() const
{
    return FileUtils::get_file_size(m_path);
}

}  // namespace hestia