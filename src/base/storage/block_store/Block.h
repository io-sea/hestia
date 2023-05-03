#pragma once

#include "Extent.h"
#include "ReadableBufferView.h"

namespace hestia {

/**
 * @brief Representation of a 'Block' of data in a container - specified by a container Extent and a buffer
 *
 * This class is an in-memory data storage container. It holds data in a buffer
 * and metadata in the form of an Extent, detailing where the data resides
 * relative to a parent container - mostly intended to be a BlockList.
 */
class Block {
  public:
    /**
     * Constructor
     *
     * @param extent Where the block resides in a parent container
     * @param buffer Input buffer - will be copied into the Block's buffer
     * @param offset Offset into the input buffer to start copying at
     */
    Block(
        const Extent& extent,
        const ReadableBufferView& buffer,
        std::size_t offset = 0);

    /**
     * Return the block's data
     * @return the block's data
     */
    const std::vector<char>& data() const;

    /**
     * Return a string based representation of the block - intended for
     * debugging
     * @return a string based representation of the block
     */
    std::string dump() const;

    /**
     * True if the block's extent equals the input extent
     * @param extent the input extent to check againsts
     * @return True if the block's extent equals the input extent
     */
    bool equals(const Extent& extent) const;

    /**
     * Return the extent of this block in its parent
     * @return the extent of this block in its parent
     */
    const Extent& extent() const;

    /**
     * Return True if the block's extent includes the input extent
     * @param extent the input extent to check againsts
     * @return True if the block's extent includes the input extent
     */
    bool includes(const Extent& extent) const;

    /**
     * Return True if the block's extent overlaps the input extent
     * @param extent the input extent to check againsts
     * @return True if the block's extent overlaps the input extent
     */
    bool overlaps(const Extent& extent) const;

    /**
     * Set the block's data to the input buffer - the block will expand to
     * accomodate the buffer, but its 'extent' remains source of truth on size.
     * @param buffer the input buffer to copy from
     * @param offset offset into the buffer to start copying from
     */
    void set_data(const ReadableBufferView& buffer, std::size_t offset = 0);

    bool operator<(const Block& other) const
    {
        return m_extent < other.m_extent;
    }

  private:
    Extent m_extent;
    std::vector<char> m_data;
};
}  // namespace hestia