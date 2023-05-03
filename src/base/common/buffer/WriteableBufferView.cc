#include "WriteableBufferView.h"

namespace hestia {
WriteableBufferView::WriteableBufferView(std::vector<char>& buffer) :
    BufferView(buffer.size()), m_data(buffer.data())
{
}

WriteableBufferView::WriteableBufferView(void* buffer, std::size_t length) :
    BufferView(length), m_data(reinterpret_cast<char*>(buffer))
{
}

char* WriteableBufferView::data()
{
    return m_data;
}

void* WriteableBufferView::as_void()
{
    return reinterpret_cast<void*>(m_data);
}
}  // namespace hestia