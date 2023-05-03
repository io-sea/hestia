#pragma once

#include "StreamSink.h"
#include "WriteableBufferView.h"

namespace hestia {
class InMemoryStreamSink : public StreamSink {
  public:
    using Ptr = std::unique_ptr<InMemoryStreamSink>;

    using Status   = std::pair<bool, std::size_t>;
    using sinkFunc = std::function<Status(
        const ReadableBufferView& buffer, std::size_t offset)>;

    InMemoryStreamSink(const WriteableBufferView& buffer);
    InMemoryStreamSink(sinkFunc sink);

    static Ptr create(const WriteableBufferView& buffer);
    static Ptr create(sinkFunc sink);

    IOResult write(const ReadableBufferView& buffer) noexcept override;

  private:
    IOResult write_to_buffer(const ReadableBufferView& buffer);
    IOResult write_to_sink_func(const ReadableBufferView& buffer);

    sinkFunc m_sink_func;
    std::size_t m_write_buffer_offset{0};
    WriteableBufferView m_write_buffer;
};
}  // namespace hestia