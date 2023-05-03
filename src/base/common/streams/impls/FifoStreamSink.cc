#include "FifoStreamSink.h"

#include "Logger.h"
#include "SystemUtils.h"

namespace hestia {
FifoStreamSink::FifoStreamSink()
{
    if (const auto status = SystemUtils::open_pipe(m_descriptors);
        !status.ok()) {
        const std::string msg =
            "Failed to initialize pipe for fifo sink - " + status.str();
        set_state(StreamState::State::ERROR, msg);
    }
}

FifoStreamSink::Ptr FifoStreamSink::create()
{
    return std::make_unique<FifoStreamSink>();
}

FifoStreamSink::~FifoStreamSink()
{
    close();
}

handle_t FifoStreamSink::get_read_descriptor() const
{
    return m_descriptors[0];
}

IOResult FifoStreamSink::write(const ReadableBufferView& buffer) noexcept
{
    const auto& [status, num_written] =
        SystemUtils::do_write(m_descriptors[1], buffer.data(), buffer.length());
    if (!status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "Error during fifo sink write - " + status.str());
        return {get_state(), 0};
    }

    if (num_written < buffer.length()) {
        std::string msg = "Failed to write full fifo sink buffer. Wrote: "
                          + std::to_string(num_written);
        msg += " of " + std::to_string(buffer.length()) + " bytes.";
        set_state(StreamState::State::ERROR, msg);
    }
    return {get_state(), num_written};
}

void FifoStreamSink::close()
{
    if (const auto status = SystemUtils::do_close(m_descriptors[1]);
        !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "Failed to close fifo sink pipe - " + status.str());
    }
}
}  // namespace hestia