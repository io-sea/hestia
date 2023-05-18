#pragma once

#include <string>

namespace hestia {

/**
 * @brief The state of a StreamIO
 *
 * This is mostly a convenience class - so consumers can easily check if
 * a Stream operations is 'ok()' and handle error messages.
 */
struct StreamState {
  public:
    enum class State { READY, FINISHED, ERROR };

    StreamState() = default;

    /**
     * Constructor
     *
     * @param state The internal state
     * @param message An error message - if appropriate
     */
    StreamState(State state, const std::string& message = {});

    /**
     * Return true if we are in the 'FINISHED' state
     *
     * @return true if we are in the 'FINISHED' state
     */
    bool finished() const;

    /**
     * Return an error message if there is one
     *
     * @return an error message if there is one
     */
    const std::string& message() const;

    /**
     * Return true if the stream is not in ERROR state
     *
     * @return true if the stream is not in ERROR state
     */
    bool ok() const;

    /**
     * Return the state as a string - intended for debugging
     *
     * @return the state as a string - intended for debugging
     */
    std::string to_string() const;

    /**
     * Convert the internal state enum to string
     *
     * @param state internal state enum
     * @return the converted state enum to string
     */
    static std::string to_string(State state);

  private:
    State m_state{State::READY};
    std::string m_message;
};

/**
 * @brief Provides a combination of the stream state and details on the conducted IO operation
 *
 * This class returns both the stream state and bytes transferred in an IO
 * operation. It could be extended for more general IO statistics.
 */
struct IOResult {

    /**
     * Return true if the stream is not in ERROR state
     *
     * @return true if the stream is not in ERROR state
     */
    bool ok() const { return m_state.ok(); }

    /**
     * Return true if we are in the 'FINISHED' state
     *
     * @return true if we are in the 'FINISHED' state
     */
    bool finished() const { return m_state.finished(); }

    StreamState m_state;
    std::size_t m_num_transferred{0};
};
}  // namespace hestia
