#pragma once

#include "StreamSource.h"
#include "SystemUtils.h"

namespace hestia {
class FifoStreamSource : public StreamSource {
  public:
    using Ptr = std::unique_ptr<FifoStreamSource>;
    FifoStreamSource();

    static Ptr create();

    virtual ~FifoStreamSource();

    handle_t get_write_descriptor() const;

    [[nodiscard]] IOResult read(WriteableBufferView& buffer) noexcept override;

  private:
    void close();

    static constexpr std::size_t num_descriptors = 2;
    handle_t m_descriptors[num_descriptors]{-1, -1};
};
}  // namespace hestia