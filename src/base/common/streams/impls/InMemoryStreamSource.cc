#include "InMemoryStreamSource.h"

namespace hestia {

InMemoryStreamSource::InMemoryStreamSource(const ReadableBufferView& buffer) :
    m_readable_buffer(buffer)
{
    m_size = buffer.length();
}

InMemoryStreamSource::InMemoryStreamSource(sourceFunc source) :
    m_source_func(source)
{
}

InMemoryStreamSource::Ptr InMemoryStreamSource::create(
    const ReadableBufferView& buffer)
{
    return std::make_unique<InMemoryStreamSource>(buffer);
}

InMemoryStreamSource::Ptr InMemoryStreamSource::create(sourceFunc source)
{
    return std::make_unique<InMemoryStreamSource>(source);
}

IOResult InMemoryStreamSource::read(
    WriteableBufferView& writeable_buffer) noexcept
{
    if (m_source_func) {
        return read_from_source_func(writeable_buffer);
    }
    else {
        return read_from_buffer(writeable_buffer);
    }
}

IOResult InMemoryStreamSource::read_from_buffer(
    WriteableBufferView& writeable_buffer)
{
    const auto read_buffer_remainder =
        m_readable_buffer.length() - m_read_buffer_offset;
    for (std::size_t idx = 0; idx < writeable_buffer.length(); idx++) {
        if (idx == read_buffer_remainder) {
            break;
        }
        writeable_buffer.data()[idx] =
            m_readable_buffer.data()[m_read_buffer_offset];
        m_read_buffer_offset++;
    }

    if (writeable_buffer.length() >= read_buffer_remainder) {
        set_state(StreamState::State::FINISHED);
        return {get_state(), read_buffer_remainder};
    }
    return {get_state(), writeable_buffer.length()};
}

void InMemoryStreamSource::seek_to(std::size_t offset)
{
    m_read_buffer_offset = offset;
    set_state(StreamState::State::READY);
}

IOResult InMemoryStreamSource::read_from_source_func(
    WriteableBufferView& writeable_buffer)
{
    const auto& [status, bytes_read] =
        m_source_func(writeable_buffer, m_read_buffer_offset);
    m_read_buffer_offset += bytes_read;

    if (!status) {
        set_state(StreamState::State::ERROR);
        return {get_state(), 0};
    }

    if (writeable_buffer.length() >= bytes_read) {
        set_state(StreamState::State::FINISHED);
        return {get_state(), bytes_read};
    }
    return {get_state(), bytes_read};
}
}  // namespace hestia