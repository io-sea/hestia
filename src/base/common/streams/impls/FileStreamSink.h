#pragma once

#include "File.h"
#include "StreamSink.h"

namespace hestia {
class FileStreamSink : public StreamSink {
  public:
    using Ptr = std::unique_ptr<FileStreamSink>;
    FileStreamSink(const File::Path& path);

    FileStreamSink(int fd, std::size_t length);

    static Ptr create(const File::Path& path);

    static Ptr create(int fd, std::size_t length);

    virtual ~FileStreamSink();

    [[nodiscard]] IOResult write(
        const ReadableBufferView& buffer) noexcept override;

  private:
    void close();

    IOResult write_to_file(const ReadableBufferView& buffer) noexcept;

    IOResult write_to_handle(const ReadableBufferView& buffer) noexcept;

    File m_file;
    int m_fd{-1};

    std::size_t m_length{0};
    std::size_t m_write_amount{0};
};
}  // namespace hestia