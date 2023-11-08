#include "Stream.h"

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"

#include "Logger.h"
#include "StreamState.h"

#include <iostream>
#include <stdexcept>

namespace hestia {

Stream::Ptr Stream::create()
{
    return std::make_unique<Stream>();
}

StreamState Stream::reset()
{
    StreamState stream_state;
    if (m_sink) {
        if (const auto io_state = m_sink->finish(); !io_state.ok()) {
            LOG_ERROR(
                "Stream sink in bad state on reset: " << io_state.message());
            stream_state = {StreamState::State::ERROR, io_state.message()};
        }
        m_sink.reset();
    }

    if (m_source) {
        if (const auto io_state = m_source->finish(); !io_state.ok()) {
            LOG_ERROR(
                "Stream source in bad state on reset: " << io_state.message());
            stream_state = {StreamState::State::ERROR, io_state.message()};
        }
        m_source.reset();
    }

    if (m_progress_func) {
        m_last_progress_call = 0;
        m_progress_func      = nullptr;
    }
    m_transfer_interval = 0;
    m_transfer_progress = 0;

    if (m_completion_func) {
        m_completion_func(stream_state);
        m_completion_func = nullptr;
    }
    return stream_state;
}

bool Stream::supports_source_seek() const
{
    return m_source && m_source->supports_seek();
}

void Stream::seek_source_to(std::size_t offset)
{
    if (supports_source_seek()) {
        m_transfer_progress = offset;
        m_source->seek_to(offset);
    }
}

void Stream::set_completion_func(completionFunc func)
{
    m_completion_func = func;
}

void Stream::set_progress_func(std::size_t interval, progressFunc func)
{
    m_transfer_interval = interval;
    m_progress_func     = func;
}

std::size_t Stream::get_sink_size() const
{
    return bool(m_sink) ? m_sink->get_size() : 0;
}

std::size_t Stream::get_source_size() const
{
    return bool(m_source) ? m_source->get_size() : 0;
}

void Stream::set_source_size(std::size_t size)
{
    if (m_source) {
        m_source->set_size(size);
    }
}

bool Stream::waiting_for_content() const
{
    if (!m_sink) {
        return false;
    }
    if (m_sink->get_state().finished()) {
        return false;
    }
    return true;
}

std::size_t Stream::get_num_transferred() const
{
    return m_transfer_progress;
}

bool Stream::has_content() const
{
    if (bool(m_source) && m_source->get_state().finished()) {
        return false;
    }
    return get_source_size() > 0;
}

bool Stream::has_source() const
{
    return bool(m_source);
}

bool Stream::supports_progress_func() const
{
    return m_progress_func != nullptr && m_transfer_interval > 0;
}

StreamState Stream::flush(std::size_t block_size) noexcept
{
    LOG_INFO("Starting stream flush");
    if (!m_sink || !m_source) {
        const std::string msg =
            "Attempted to flush stream without both sink and source";
        LOG_ERROR(msg);
        return {StreamState::State::ERROR, msg};
    }

    std::vector<char> buffer(block_size, 0);

    StreamState state;
    WriteableBufferView writeable_buffer(buffer);
    while (true) {
        auto read_result = m_source->read(writeable_buffer);
        if (!read_result.ok()) {
            state = {StreamState::State::ERROR, read_result.m_state.message()};
            break;
        }

        ReadableBufferView readable_buffer(
            &buffer[0], read_result.m_num_transferred);
        auto write_result = m_sink->write(readable_buffer);
        if (!write_result.ok()) {
            state = {StreamState::State::ERROR, write_result.m_state.message()};
            break;
        }

        m_transfer_progress += write_result.m_num_transferred;
        if (supports_progress_func()
            && m_transfer_progress
                   >= m_transfer_interval + m_last_progress_call) {
            m_last_progress_call = m_transfer_progress.load();
            LOG_INFO("About to call progress func");
            m_progress_func(m_transfer_progress);
        }

        if (read_result.finished()) {
            state = {StreamState::State::FINISHED};
            state.set_num_transferred(m_transfer_progress);
            break;
        }
    }
    if (auto reset_state = reset(); !reset_state.ok()) {
        state = reset_state;
    }
    return state;
}

IOResult Stream::read(WriteableBufferView& buffer) noexcept
{
    if (!m_source) {
        const std::string msg = "Attempted to read without a source";
        LOG_ERROR(msg);
        return {{StreamState::State::ERROR, msg}, 0};
    }

    const auto result = m_source->read(buffer);
    if (!result.ok()) {
        return result;
    }

    m_transfer_progress += result.m_num_transferred;
    if (supports_progress_func()
        && m_transfer_progress >= m_transfer_interval + m_last_progress_call) {
        m_last_progress_call = m_transfer_progress.load();
        m_progress_func(m_transfer_progress);
    }
    return result;
}

IOResult Stream::write(const ReadableBufferView& buffer) noexcept
{
    if (!m_sink) {
        const std::string msg = "Attempted to write without a sink";
        LOG_ERROR(msg);
        return {{StreamState::State::ERROR, msg}, 0};
    }

    const auto result = m_sink->write(buffer);
    if (!result.ok()) {
        return result;
    }

    m_transfer_progress += result.m_num_transferred;
    if (supports_progress_func()
        && m_transfer_progress >= m_transfer_interval + m_last_progress_call) {
        m_last_progress_call = m_transfer_progress.load();
        m_progress_func(m_transfer_progress);
    }
    return result;
}

void Stream::set_source(StreamSource::Ptr source)
{
    if (m_source) {
        throw std::runtime_error(
            "Setting source on a stream with existing source. reset() the stream first.");
    }
    m_source = std::move(source);
}

void Stream::set_sink(StreamSink::Ptr sink)
{
    if (m_sink) {
        throw std::runtime_error(
            "Setting sink on a stream with existing sink. reset() the stream first.");
    }
    m_sink = std::move(sink);
}

}  // namespace hestia