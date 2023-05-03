#pragma once

#include "BufferView.h"

#include <string>
#include <vector>

namespace hestia {

/**
 * @brief A wrapper for a Buffer with Write-Only semantics
 *
 * A wrapper for a buffer that allows writing only.
 */
class WriteableBufferView : public BufferView {
  public:
    WriteableBufferView() = default;

    /**
     * Constructor
     *
     * @param buffer A char-vec representation of the buffer
     */
    WriteableBufferView(std::vector<char>& buffer);

    /**
     * Constructor
     *
     * @param buffer A raw buffer
     * @param length The buffer length
     */
    WriteableBufferView(void* buffer, std::size_t length);

    virtual ~WriteableBufferView() = default;

    /**
     * Return the buffer as a void-ptr as might be needed by some external APIs.
     * Avoid it internally.
     *
     * @return the buffer as a void-ptr
     */
    void* as_void();

    /**
     * Return the buffer as a char type - convenience for indexing
     *
     * @return the buffer as a char type
     */
    char* data();

  private:
    char* m_data{nullptr};
};

}  // namespace hestia