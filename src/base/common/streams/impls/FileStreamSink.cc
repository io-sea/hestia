#include "FileStreamSink.h"

#include "FileUtils.h"

namespace hestia {
FileStreamSink::FileStreamSink(const File::Path& path) : m_file(path) {}

FileStreamSink::Ptr FileStreamSink::create(const File::Path& path)
{
    return std::make_unique<FileStreamSink>(path);
}

FileStreamSink::~FileStreamSink()
{
    close();
}

IOResult FileStreamSink::write(const ReadableBufferView& buffer) noexcept
{
    if (const auto state = get_state(); !state.ok()) {
        return {state, 0};
    }

    if (const auto status = m_file.write(buffer.data(), buffer.length());
        !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSink write failed: " + status.str());
    }
    return {get_state(), buffer.length()};
}

void FileStreamSink::close()
{
    if (const auto status = m_file.close(); !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "FileStreamSink close failed: " + status.str());
    }
}
}  // namespace hestia