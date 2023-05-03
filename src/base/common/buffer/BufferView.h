#pragma once

#include <cstddef>

namespace hestia {

/**
 * @brief A convenience wrapper for a buffer
 *
 * Convenience wrapper for a buffer - intended to keep the buffer length and
 * data in the same object for safer consumption. Data is handled in derived
 * classes.
 *
 * It does not handle the buffer lifetime or ownership.
 */
class BufferView {
  public:
    /**
     * Constructor
     *
     * @param length Length of the buffer
     */
    BufferView(std::size_t length = 0) : m_length(length) {}

    virtual ~BufferView() = default;

    std::size_t length() const { return m_length; }

  protected:
    std::size_t m_length{0};
};

}  // namespace hestia