#pragma once

#include "StreamState.h"

#include <future>
#include <mutex>

namespace hestia {

/**
 * @brief Base class with common methods for Stream Sinks and Sources
 *
 * This class has common functionality for Sinks and Sources. Sinks provide
 * data to Stream consumers and Sources consume it.
 *
 * A StreamIO has state (OK, ERROR, FINISHED etc) which can be accessed on
 * multiple threads, as state updates actual data transfer can happen on a
 * thread different to the initial requesting one.
 *
 */
class StreamIO {
  public:
    virtual ~StreamIO() = default;

    /**
     * Does the IO have non-zero size
     *
     * @return true if the IO has non-zero size
     */
    bool empty() const;

    /**
     * Instruct IO to finish - if there is data transfer on another thread we
     * wait until its state 'producer' is finished.
     *
     * @return the final state of the stream
     */
    [[nodiscard]] virtual StreamState finish() noexcept;

    /**
     * Get the size of the data we are providing or consuming
     * @return the size of the data we are providing or consuming
     */
    std::size_t get_size() const;

    /**
     * Get the state of the IO (i.e. OK, ERROR, FINISHED or similar)
     * @return the state of the IO
     */
    StreamState get_state() const;

    /**
     * Set the size of the data available to or requested by the IO
     * @return the size of the data available to or requested by the IO
     */
    void set_size(std::size_t size);

    /**
     * Set the 'state producer' - if the data transfer is happening on another
     * thread a 'finish' call will wait for this 'future' before proceeding.
     * @param producer A future - finish calls will wait for it
     */
    void set_producer(std::future<int> producer);

  protected:
    void set_state(StreamState::State state, const std::string& message = {});

    std::future<int> m_producer;
    std::size_t m_size{0};

  private:
    mutable std::mutex m_state_mutex;
    StreamState m_state;  // Only access under lock. Use set/get helper methods.
};
}  // namespace hestia