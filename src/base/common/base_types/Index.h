#pragma once

#include <cstddef>

namespace hestia {
class Index {
  public:
    enum class State { UNSET, SET };

    Index() = default;

    Index(std::size_t value) : m_state(State::SET), m_value(value) {}

    bool is_set() const { return m_state == State::SET; }

    std::size_t value() const { return m_value; }

    State m_state{State::UNSET};
    std::size_t m_value{0};
};
}  // namespace hestia