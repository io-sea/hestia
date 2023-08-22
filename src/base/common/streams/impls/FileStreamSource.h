#pragma once

#include "File.h"
#include "StreamSource.h"

namespace hestia {
class FileStreamSource : public StreamSource {
  public:
    using Ptr = std::unique_ptr<FileStreamSource>;

    FileStreamSource(const File::Path& path);

    FileStreamSource(int fd, std::size_t length);

    static Ptr create(const File::Path& path);

    static Ptr create(int fd, std::size_t length);

    virtual ~FileStreamSource();

    [[nodiscard]] IOResult read(WriteableBufferView& buffer) noexcept override;

    bool supports_seek() const override { return true; }

    void seek_to(std::size_t offset) override;

  private:
    void close();

    IOResult read_from_file(WriteableBufferView& buffer) noexcept;

    IOResult read_from_handle(WriteableBufferView& buffer) noexcept;

    File m_file;
    int m_fd{-1};
    std::size_t m_length{0};
};
}  // namespace hestia