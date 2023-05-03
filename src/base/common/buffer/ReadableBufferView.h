#pragma once

#include "BufferView.h"

#include <string>
#include <vector>

namespace hestia {

/**
 * @brief A wrapper for a Buffer with Read-Only semantics
 *
 * A wrapper for a buffer that allows reading only.
 */
class ReadableBufferView : public BufferView {
  public:
    ReadableBufferView() = default;

    /**
     * Constructor
     *
     * @param buffer A string representation of the buffer
     */
    ReadableBufferView(const std::string& buffer);

    /**
     * Constructor
     *
     * @param buffer A char-vec representation of the buffer
     */
    ReadableBufferView(const std::vector<char>& buffer);

    /**
     * Constructor
     *
     * @param buffer A raw buffer
     * @param length The buffer length
     */
    ReadableBufferView(const void* buffer, std::size_t length);

    virtual ~ReadableBufferView() = default;

    /**
     * Return the buffer as a void-ptr as might be needed by some external APIs.
     * Avoid it internally.
     *
     * @return the buffer as a void-ptr
     */
    const void* as_void() const;

    /**
     * Return the buffer as a char type - convenience for indexing
     *
     * @return the buffer as a char type
     */
    const char* data() const;

    /**
     * Return a new view using an offset into the current one and length
     *
     * @param offset offset into the current buffer for the new view
     * @param length size of the new view
     * @return a view at the specified offset and length into the current one
     */
    ReadableBufferView slice(std::size_t offset, std::size_t length) const;

  private:
    const char* m_data{nullptr};
};
}  // namespace hestia