#include "Block.h"

namespace hestia {
Block::Block(
    const Extent& extent,
    const ReadableBufferView& buffer,
    std::size_t offset) :
    m_extent(extent)
{
    set_data(buffer, offset);
}

const Extent& Block::extent() const
{
    return m_extent;
}

const std::vector<char>& Block::data() const
{
    return m_data;
}

bool Block::equals(const Extent& extent) const
{
    return m_extent == extent;
}

bool Block::includes(const Extent& extent) const
{
    return m_extent.includes(extent);
}

bool Block::overlaps(const Extent& extent) const
{
    return m_extent.overlaps(extent);
}

void Block::set_data(const ReadableBufferView& buffer, std::size_t offset)
{
    if (m_data.empty()) {
        if (offset == 0) {
            m_data = std::vector<char>(buffer.length(), 0);
            for (std::size_t idx = 0; idx < buffer.length(); idx++) {
                m_data[idx] = buffer.data()[idx];
            }
        }
        return;
    }

    if (buffer.length() + offset > m_data.size()) {
        if (auto excess = buffer.length() + offset - m_data.size();
            excess > 0) {
            m_data.resize(m_data.size() + excess);
        }
    }
    for (std::size_t idx = 0; idx < buffer.length(); idx++) {
        m_data[idx + offset] = buffer.data()[idx];
    }
}

std::string Block::dump() const
{
    std::string ret;
    ret += "OFFSET" + std::to_string(m_extent.m_offset);
    ret += "LENGTH" + std::to_string(m_extent.m_length);
    ret += "DATA" + std::string(m_data.begin(), m_data.end());
    return ret;
}
}  // namespace hestia