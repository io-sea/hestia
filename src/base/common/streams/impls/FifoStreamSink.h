#pragma once

#include "StreamSink.h"
#include "SystemUtils.h"

namespace hestia {
class FifoStreamSink : public StreamSink {
  public:
    using Ptr = std::unique_ptr<FifoStreamSink>;
    FifoStreamSink();

    static Ptr create();

    virtual ~FifoStreamSink();

    handle_t get_read_descriptor() const;

    [[nodiscard]] IOResult write(
        const ReadableBufferView& buffer) noexcept override;

  private:
    void close();

  private:
    static constexpr std::size_t num_descriptors = 2;
    handle_t m_descriptors[num_descriptors]      = {-1, -1};
};
}  // namespace hestia