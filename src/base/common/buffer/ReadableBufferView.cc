#include "ReadableBufferView.h"

namespace hestia {
ReadableBufferView::ReadableBufferView(const std::string& buffer) :
    BufferView(buffer.size()), m_data(buffer.data())
{
}

ReadableBufferView::ReadableBufferView(const std::vector<char>& buffer) :
    BufferView(buffer.size()), m_data(buffer.data())
{
}

ReadableBufferView::ReadableBufferView(const void* buffer, std::size_t length) :
    BufferView(length), m_data(reinterpret_cast<const char*>(buffer))
{
}

ReadableBufferView ReadableBufferView::slice(
    std::size_t offset, std::size_t length) const
{
    return ReadableBufferView(m_data + offset, length);
}

const char* ReadableBufferView::data() const
{
    return m_data;
}

const void* ReadableBufferView::as_void() const
{
    return reinterpret_cast<const void*>(m_data);
}
}  // namespace hestia