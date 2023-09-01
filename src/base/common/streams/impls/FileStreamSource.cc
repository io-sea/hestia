#include "FileStreamSource.h"

#include "SystemUtils.h"

#include <iostream>

namespace hestia {
FileStreamSource::FileStreamSource(const File::Path& path) : m_file(path)
{
    m_size = m_file.get_size();
}

FileStreamSource::Ptr FileStreamSource::create(const File::Path& path)
{
    return std::make_unique<FileStreamSource>(path);
}

FileStreamSource::Ptr FileStreamSource::create(int fd, std::size_t length)
{
    return std::make_unique<FileStreamSource>(fd, length);
}

FileStreamSource::FileStreamSource(int fd, std::size_t length) :
    m_fd(fd), m_length(length)
{
    (void)m_length;
}

FileStreamSource::~FileStreamSource()
{
    close();
}

void FileStreamSource::seek_to(std::size_t offset)
{
    m_file.seek_to(offset);
    set_state(StreamState::State::READY);
}

IOResult FileStreamSource::read(WriteableBufferView& buffer) noexcept
{
    if (m_fd > -1) {
        return read_from_handle(buffer);
    }
    else {
        return read_from_file(buffer);
    }
}

IOResult FileStreamSource::read_from_file(WriteableBufferView& buffer) noexcept
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

IOResult FileStreamSource::read_from_handle(
    WriteableBufferView& buffer) noexcept
{
    if (const auto state = get_state(); !state.ok()) {
        return {state, 0};
    }

    const auto& [op_status, read_size] =
        SystemUtils::do_read(m_fd, buffer.data(), buffer.length());
    if (!op_status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSource: " + op_status.message());
        return {get_state(), 0};
    }

    if (read_size <= buffer.length()) {
        set_state(StreamState::State::FINISHED);
    }
    return {get_state(), read_size};
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