#pragma once

#include "File.h"
#include "StreamSink.h"

namespace hestia {
class FileStreamSink : public StreamSink {
  public:
    using Ptr = std::unique_ptr<FileStreamSink>;
    FileStreamSink(const File::Path& path);

    static Ptr create(const File::Path& path);

    virtual ~FileStreamSink();

    [[nodiscard]] IOResult write(
        const ReadableBufferView& buffer) noexcept override;

  private:
    void close();

    File m_file;
};
}  // namespace hestia