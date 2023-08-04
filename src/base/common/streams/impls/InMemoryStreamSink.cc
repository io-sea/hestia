#include "InMemoryStreamSink.h"

#include "Logger.h"

namespace hestia {
InMemoryStreamSink::InMemoryStreamSink(const WriteableBufferView& buffer) :
    m_write_buffer(buffer)
{
    m_size = buffer.length();
}

InMemoryStreamSink::InMemoryStreamSink(sinkFunc sink) : m_sink_func(sink) {}

InMemoryStreamSink::Ptr InMemoryStreamSink::create(
    const WriteableBufferView& buffer)
{
    return std::make_unique<InMemoryStreamSink>(buffer);
}

InMemoryStreamSink::Ptr InMemoryStreamSink::create(sinkFunc sink)
{
    return std::make_unique<InMemoryStreamSink>(sink);
}

IOResult InMemoryStreamSink::write(
    const ReadableBufferView& read_buffer) noexcept
{
    if (m_sink_func) {
        return write_to_sink_func(read_buffer);
    }
    else {
        return write_to_buffer(read_buffer);
    }
}

IOResult InMemoryStreamSink::write_to_buffer(
    const ReadableBufferView& read_buffer)
{
    std::size_t starting_offset = m_write_buffer_offset;
    for (std::size_t idx = 0; idx < read_buffer.length(); idx++) {
        if (m_write_buffer_offset == m_write_buffer.length()) {
            set_state(StreamState::State::FINISHED);
            break;
        }
        m_write_buffer.data()[m_write_buffer_offset] = read_buffer.data()[idx];
        m_write_buffer_offset++;
    }
    return {get_state(), m_write_buffer_offset - starting_offset};
}

IOResult InMemoryStreamSink::write_to_sink_func(
    const ReadableBufferView& read_buffer)
{
    const auto& [status, bytes_written] =
        m_sink_func(read_buffer, m_write_buffer_offset);
    if (!status) {
        set_state(StreamState::State::ERROR);
        return {get_state(), 0};
    }

    m_write_buffer_offset += bytes_written;

    if (read_buffer.length() >= bytes_written) {
        set_state(StreamState::State::FINISHED);
        return {get_state(), bytes_written};
    }
    return {get_state(), read_buffer.length()};
}

}  // namespace hestia