#include "StreamState.h"

namespace hestia {
StreamState::StreamState(State state, const std::string& message) :
    m_state(state), m_message(message)
{
}

bool StreamState::finished() const
{
    return m_state == State::FINISHED || m_state == State::ERROR;
}

bool StreamState::ok() const
{
    return m_state != State::ERROR;
}

std::string StreamState::to_string() const
{
    return to_string(m_state);
}

std::string StreamState::to_string(State state)
{
    switch (state) {
        case State::READY:
            return "READY";
        case State::FINISHED:
            return "FINISHED";
        case State::ERROR:
            return "ERROR";
        default:
            return "UNKOWN";
    }
}

const std::string& StreamState::message() const
{
    return m_message;
}
}  // namespace hestia