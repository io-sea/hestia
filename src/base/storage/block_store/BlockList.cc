#include "BlockList.h"

namespace hestia {
void BlockList::add_block(
    const hestia::Extent& extent,
    const ReadableBufferView& buffer,
    std::size_t offset)
{
    if (const auto [iter, added] =
            m_blocks.emplace(extent.m_offset, Block(extent, buffer, offset));
        !added) {
        iter->second = Block(extent, buffer, offset);
    }
}

hestia::Extent BlockList::get_extent_bounds() const
{
    if (m_blocks.empty()) {
        return {};
    }

    auto bounds     = m_blocks.begin()->second.extent();
    const auto end  = m_blocks.rbegin()->second.extent().get_end();
    bounds.m_length = end - bounds.m_offset;
    return bounds;
}

void BlockList::write(const Extent& extent, const ReadableBufferView& buffer)
{
    auto working_ext = extent;
    if (extent.empty()) {
        working_ext = {0, buffer.length()};
    }

    if (auto bounds = get_extent_bounds();
        !bounds.includes_or_overlaps(working_ext)) {
        add_block(working_ext, buffer);
        return;
    }

    for (auto& [offset, block] : m_blocks) {
        if (block.equals(working_ext)) {
            const auto delta_offset = working_ext.m_offset - extent.m_offset;
            add_block(working_ext, buffer, delta_offset);
            break;
        }
        else if (block.includes(working_ext)) {
            const auto delta_offset = working_ext.m_offset - extent.m_offset;
            const auto block_offset =
                working_ext.m_offset - block.extent().m_offset;
            block.set_data(
                buffer.slice(delta_offset, working_ext.m_length), block_offset);
            break;
        }
        else if (block.overlaps(working_ext)) {
            const auto overlap_length =
                block.extent().get_overlapping_length(working_ext);
            const auto delta_offset = working_ext.m_offset - extent.m_offset;
            const auto block_offset =
                working_ext.m_offset - block.extent().m_offset;
            block.set_data(
                buffer.slice(delta_offset, delta_offset + overlap_length),
                block_offset);

            working_ext.m_offset += overlap_length;
            working_ext.m_length -= overlap_length;
            if (working_ext.m_length == 0) {
                return;
            }
        }
    }
    if (working_ext.m_offset == extent.m_offset
        && working_ext.m_length == extent.m_length) {
        add_block(working_ext, buffer);
    }
}

std::pair<bool, std::size_t> BlockList::read(
    const Extent& extent, WriteableBufferView& buffer) const
{
    auto working_ext = extent;
    if (extent.empty()) {
        const auto bounds = get_extent_bounds();
        working_ext       = {0, bounds.m_offset + bounds.m_length};
    }

    if (auto bounds = get_extent_bounds();
        !bounds.includes_or_overlaps(working_ext)) {
        return {false, 0};
    }

    std::size_t buffer_loc{0};
    std::size_t bytes_read{0};
    for (const auto& [offset, block] : m_blocks) {
        if (block.equals(working_ext)) {
            for (std::size_t idx = 0; idx < working_ext.m_length; idx++) {
                buffer.data()[buffer_loc] = block.data()[idx];
                buffer_loc++;
            }
            bytes_read += working_ext.m_length;
            return {true, bytes_read};
        }
        else if (block.includes(working_ext)) {
            const auto left_offset =
                working_ext.m_offset - block.extent().m_offset;
            for (std::size_t idx = 0; idx < working_ext.m_length; idx++) {
                buffer.data()[buffer_loc] = block.data()[left_offset + idx];
                buffer_loc++;
            }
            bytes_read += working_ext.m_length;
            return {true, bytes_read};
        }
        else if (block.overlaps(working_ext)) {
            const auto left_offset =
                working_ext.m_offset - block.extent().m_offset;
            const auto overlap_length =
                block.extent().get_overlapping_length(working_ext);

            for (std::size_t idx = 0; idx < overlap_length; idx++) {
                buffer.data()[buffer_loc] = block.data()[left_offset + idx];
                buffer_loc++;
            }
            working_ext.m_offset += overlap_length;
            working_ext.m_length -= overlap_length;
            bytes_read += overlap_length;
            if (working_ext.m_length == 0) {
                return {true, bytes_read};
            }
        }
    }
    return {false, 0};
}

std::string BlockList::dump() const
{
    std::string ret;
    for (const auto& [offset, block] : m_blocks) {
        ret += block.dump() + "\n";
    }
    return ret;
}
}  // namespace hestia