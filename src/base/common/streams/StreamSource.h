#pragma once

#include "StreamIO.h"
#include "WriteableBufferView.h"

namespace hestia {

/**
 * @brief Consumes data from a stream
 *
 * This class is a StreamIO which can provide data to a stream.
 */
class StreamSource : public StreamIO {
  public:
    using Ptr = std::unique_ptr<StreamSource>;

    virtual ~StreamSource() = default;

    /**
     * Derived classes can use this to specify how to read from a source to the
     * provided buffer
     *
     * @param buffer the buffer that the source should write to
     * @return the status of the read operation
     */
    [[nodiscard]] virtual IOResult read(
        WriteableBufferView& buffer) noexcept = 0;
};
}  // namespace hestia