#include "StreamIO.h"

#include "Logger.h"

#include <stdexcept>

namespace hestia {

bool StreamIO::empty() const
{
    return m_size == 0;
}

StreamState StreamIO::finish() noexcept
{
    if (m_producer.valid()) {
        try {
            m_producer.get();
        }
        catch (std::exception& e) {
            LOG_ERROR("Exception finishing Stream IO: " << e.what());
            set_state(StreamState::State::ERROR, e.what());
        }
        catch (...) {
            LOG_ERROR("Unknown Exception finishing Stream IO");
            set_state(StreamState::State::ERROR, "Unknown exception.");
        }
    }
    if (m_state.ok()) {
        set_state(StreamState::State::FINISHED);
    }
    return get_state();
}

std::size_t StreamIO::get_size() const
{
    return m_size;
}

StreamState StreamIO::get_state() const
{
    StreamState state;
    {
        std::scoped_lock guard(m_state_mutex);
        state = m_state;
    }
    return state;
}

void StreamIO::set_state(StreamState::State state, const std::string& message)
{
    {
        std::scoped_lock guard(m_state_mutex);
        m_state = StreamState(state, message);
    }

    if (!message.empty()) {
        LOG_ERROR(message);
    }
}

void StreamIO::set_size(std::size_t size)
{
    m_size = size;
}

void StreamIO::set_producer(std::future<int> producer)
{
    if (m_producer.valid()) {
        throw std::runtime_error(
            "Attempted to set producer with one already set");
    }
    m_producer = std::move(producer);
}
}  // namespace hestia