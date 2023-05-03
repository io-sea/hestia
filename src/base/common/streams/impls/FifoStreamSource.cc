#include "FifoStreamSource.h"

#include "SystemUtils.h"

namespace hestia {
FifoStreamSource::FifoStreamSource()
{
    if (const auto status = SystemUtils::open_pipe(m_descriptors);
        !status.ok()) {
        const std::string msg =
            "Failed to initialize pipe for fifo source - " + status.str();
        set_state(StreamState::State::ERROR, msg);
    }
}

FifoStreamSource::Ptr FifoStreamSource::create()
{
    return std::make_unique<FifoStreamSource>();
}

handle_t FifoStreamSource::get_write_descriptor() const
{
    return m_descriptors[1];
}

FifoStreamSource::~FifoStreamSource()
{
    close();
}

IOResult FifoStreamSource::read(WriteableBufferView& buffer) noexcept
{
    const auto [status, num_read] =
        SystemUtils::do_read(m_descriptors[0], buffer.data(), buffer.length());
    if (!status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "Fifo source error during read - " + status.str());
        return {get_state(), 0};
    }

    if (num_read != buffer.length()) {
        set_state(StreamState::State::FINISHED);
    }
    return {get_state(), num_read};
}

void FifoStreamSource::close()
{
    if (const auto status = SystemUtils::do_close(m_descriptors[0]);
        !status.ok()) {
        set_state(
            StreamState::State::ERROR,
            "Failed to close fifo source pipe - " + status.str());
    }
}
}  // namespace hestia