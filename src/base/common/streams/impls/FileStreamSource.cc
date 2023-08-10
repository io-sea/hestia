#include "FileStreamSource.h"

namespace hestia {
FileStreamSource::FileStreamSource(const File::Path& path) : m_file(path)
{
    m_size = m_file.get_size();
}

FileStreamSource::Ptr FileStreamSource::create(const File::Path& path)
{
    return std::make_unique<FileStreamSource>(path);
}

FileStreamSource::~FileStreamSource()
{
    close();
}

void FileStreamSource::seek_to(std::size_t offset)
{
    m_file.seek_to(offset);
}

IOResult FileStreamSource::read(WriteableBufferView& buffer) noexcept
{
    if (const auto state = get_state(); !state.ok()) {
        return {state, 0};
    }

    const auto& [op_status, read_state] =
        m_file.read(buffer.data(), buffer.length());
    if (!op_status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSource: " + op_status.message());
        return {get_state(), 0};
    }

    if (read_state.m_finished) {
        set_state(StreamState::State::FINISHED);
    }
    return {get_state(), read_state.m_size_read};
}

void FileStreamSource::close()
{
    if (const auto status = m_file.close(); !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSource close failed: " + status.str());
    }
}
}  // namespace hestia