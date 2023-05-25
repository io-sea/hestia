#pragma once

#include "InMemoryStreamSink.h"
#include "InMemoryStreamSource.h"
#include "Stream.h"

namespace hestia::mock {
class MockWebService {
  public:
    std::size_t get_data(Stream* stream)
    {
        if (m_buffer.empty()) {
            return 0;
        }
        stream->set_source(InMemoryStreamSource::create(m_buffer));
        stream->set_source_size(m_buffer.size());
        return m_buffer.size();
    }

    void set_data(std::size_t size, Stream* stream)
    {
        m_buffer.clear();
        m_buffer.resize(size);
        stream->set_sink(InMemoryStreamSink::create(m_buffer));
    }

  private:
    std::vector<char> m_buffer;
};
}  // namespace hestia::mock