#pragma once

#include "StreamSink.h"
#include "StreamSource.h"

#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include <memory>

namespace hestia {

/**
 * @brief Convenience class to handle chunked transfer of data - the transfer can be immediate or deferred.
 *
 * This class is intended to keep Object Store interfaces clean by providing a
 * single interface over the many ways object data can be transferred.
 *
 * The Stream has a Source and a Sink, with a few ways to transfer the data
 * between them. 1) Immediate write - we can write a buffer to the steam, if
 * there is a Sink attached data will be passed to it 2) Immediate read - we can
 * read from the stream - if a Source has been attached and data is available.
 * 3) Immediate flush - we can flush from the Source to the Sink if both are
 * attached. A block size for the flush can be given. 4) Deferred flush - we can
 * dispatch flushing onto a thread and only wait for completion when we 'reset'
 * the Stream.
 *
 * Resetting the stream will both complete all transfers and detach any Sinks or
 * Sources - allowing the stream to be re-used if needed.
 *
 */
class Stream {
  public:
    using Ptr = std::unique_ptr<Stream>;

    virtual ~Stream() = default;

    /**
     * Factory Constructor
     *
     * @return A ptr to a newly created instance
     */
    static Ptr create();

    /**
     * Flush the stream - if both a Source and Sink are attached it will read
     * blocks from the former and write them to the latter.
     *
     * @param block_size The size of the blocks to 'stage' data to while doing the flush
     * @return The status of the stream - it can indicate and error state or partial data transfer
     */
    [[nodiscard]] StreamState flush(std::size_t block_size = 4000) noexcept;

    /**
     * Return the size of the data in the sink. Not all sink types may be able
     * to give a size in advance.
     *
     * @return the size of the data in the sink
     */
    std::size_t get_sink_size() const;

    /**
     * Return the size of the data in the source. Not all source types may be
     * able to give a size in advance.
     *
     * @return the size of the data in the source
     */
    std::size_t get_source_size() const;

    /**
     * True if the source size is bigger than zero. Not all source types may be
     * able to give a size in advance.
     *
     * @return True if the source size is bigger than zero.
     */
    bool has_content() const;

    /**
     * Read from an attached Source into the provided buffer
     *
     * @param buffer the buffer to be written to - its length controls the attempted read size
     * @return the state of the read operation, including error status and number of bytes read
     */
    [[nodiscard]] IOResult read(WriteableBufferView& buffer) noexcept;

    /**
     * Resets the stream. If there are operations on another thread it will wait
     * for them. It will then detach (and therefore destory) the attached Sink
     * and Source.
     *
     * @return the state of the reset operation, including error status.
     */
    [[nodiscard]] StreamState reset();

    /**
     * Sets the source for the stream - if there already is one it will be
     * destroyed.
     *
     * @param source the stream source
     */
    void set_source(StreamSource::Ptr source);

    /**
     * Sets the size of the source
     *
     * @param size the size of the source
     */
    void set_source_size(std::size_t size);

    /**
     * Sets the sink for the stream - if there already is one it will be
     * destroyed.
     *
     * @param sink the stream sink
     */
    void set_sink(StreamSink::Ptr sink);

    /**
     * True if a Sink is attached and of non-zero size
     *
     * @return True if a Sink is attached and of non-zero size
     */
    bool waiting_for_content() const;

    /**
     * Write to an attached Sink from the provided buffer
     *
     * @param buffer the buffer to read from - its length controls the attempted write size
     * @return the state of the write operation, including error status and number of bytes written
     */
    [[nodiscard]] IOResult write(const ReadableBufferView& buffer) noexcept;

  protected:
    StreamSource::Ptr m_source;
    StreamSink::Ptr m_sink;
};
}  // namespace hestia
