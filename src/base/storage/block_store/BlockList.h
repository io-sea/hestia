#pragma once

#include "Block.h"
#include "Extent.h"
#include "ReadableBufferView.h"
#include "WriteableBufferView.h"

#include <map>

namespace hestia {

/**
 * @brief A data container backed by Blocks at arbitary offsets
 *
 * The BlockList is an in-memory data container. When written to it will either
 * create a new Block to receive the data or merge it into a pre-existing block.
 * It supports writing at arbitary offsets into the container, but only reading
 * from existing extents (or data ranges).
 */
class BlockList {
  public:
    /**
     * Return a string representation of the container - intended for debugging
     * @return a string representation of the container - intended for debugging
     */
    std::string dump() const;

    /**
     * Read from the container given a certain extent
     * @param extent the extent to read from
     * @param buffer the buffer to read into
     * @return a pair with the status of the read (false if failed) and number of bytes read
     */
    std::pair<bool, std::size_t> read(
        const Extent& extent, WriteableBufferView& buffer) const;

    /**
     * Write data into the container
     * @param extent the extent to write to
     * @param buffer the buffer to read from
     */
    void write(const Extent& extent, const ReadableBufferView& buffer);

  private:
    void add_block(
        const Extent& extent,
        const ReadableBufferView& buffer,
        std::size_t offset = 0);

    hestia::Extent get_extent_bounds() const;

    std::map<std::size_t, Block> m_blocks;
};
}  // namespace hestia