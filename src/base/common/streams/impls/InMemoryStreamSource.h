#pragma once

#include "ReadableBufferView.h"
#include "StreamSource.h"

#include <functional>

namespace hestia {
class InMemoryStreamSource : public StreamSource {
  public:
    using Ptr = std::unique_ptr<InMemoryStreamSource>;

    using Status     = std::pair<bool, std::size_t>;
    using sourceFunc = std::function<Status(WriteableBufferView& buffer)>;

    InMemoryStreamSource(const ReadableBufferView& buffer);
    InMemoryStreamSource(sourceFunc source);

    static Ptr create(const ReadableBufferView& buffer);
    static Ptr create(sourceFunc source);

    IOResult read(WriteableBufferView& buffer) noexcept override;

  private:
    IOResult read_from_buffer(WriteableBufferView& buffer);
    IOResult read_from_source_func(WriteableBufferView& buffer);

    sourceFunc m_source_func;
    ReadableBufferView m_readable_buffer;
    std::size_t m_read_buffer_offset{0};
};
}  // namespace hestia