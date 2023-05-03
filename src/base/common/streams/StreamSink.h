#pragma once

#include "ReadableBufferView.h"
#include "StreamIO.h"

#include <memory>

namespace hestia {

/**
 * @brief Consumes data from a stream
 *
 * This class is a StreamIO which can consume data from a stream.
 */
class StreamSink : public StreamIO {
  public:
    using Ptr = std::unique_ptr<StreamSink>;

    virtual ~StreamSink() = default;

    /**
     * Derived classes can use this to specify how to write to the sink given
     * the provided buffer
     *
     * @param buffer the buffer that the sink should consume
     * @return the status of the write operation
     */
    [[nodiscard]] virtual IOResult write(
        const ReadableBufferView& buffer) noexcept = 0;
};
}  // namespace hestia