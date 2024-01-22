#include "FileStreamSink.h"

#include "FileUtils.h"
#include "SystemUtils.h"

#include <iostream>

namespace hestia {
FileStreamSink::FileStreamSink(const File::Path& path) : m_file(path) {}

FileStreamSink::FileStreamSink(int fd, std::size_t length) :
    m_fd(fd), m_length(length)
{
}

FileStreamSink::Ptr FileStreamSink::create(const File::Path& path)
{
    return std::make_unique<FileStreamSink>(path);
}

FileStreamSink::Ptr FileStreamSink::create(int fd, std::size_t length)
{
    return std::make_unique<FileStreamSink>(fd, length);
}

FileStreamSink::~FileStreamSink()
{
    close();
}

IOResult FileStreamSink::write(const ReadableBufferView& buffer) noexcept
{
    if (m_fd > -1) {
        return write_to_handle(buffer);
    }
    else {
        return write_to_file(buffer);
    }
}

IOResult FileStreamSink::write_to_file(
    const ReadableBufferView& buffer) noexcept
{
    if (const auto state = get_state(); !state.ok()) {
        return {state, 0};
    }

    if (const auto status = m_file.write(buffer.data(), buffer.length());
        !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSink write failed: " + status.str());
    }
    return {get_state(), buffer.length()};
}

IOResult FileStreamSink::write_to_handle(
    const ReadableBufferView& buffer) noexcept
{
    if (const auto state = get_state(); !state.ok()) {
        return {state, 0};
    }

    if (const auto status = m_file.write(buffer.data(), buffer.length());
        !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSink write failed: " + status.str());
    }

    const auto write_amount = buffer.length() + m_write_amount > m_length ?
                                  m_length - buffer.length() + m_write_amount :
                                  buffer.length();

    const auto& [op_status, write_size] =
        SystemUtils::do_write(m_fd, buffer.data(), write_amount);
    if (!op_status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSource: " + op_status.message());
        return {get_state(), 0};
    }

    if (write_size <= buffer.length() || write_size == 0) {
        set_state(StreamState::State::FINISHED);
    }

    return {get_state(), write_size};
}

void FileStreamSink::close()
{
    if (const auto status = m_file.close(); !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSink close failed: " + status.str());
    }
}
}  // namespace hestia