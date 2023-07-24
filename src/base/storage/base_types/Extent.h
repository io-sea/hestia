#pragma once

#include "Serializeable.h"

#include <string>

namespace hestia {

/**
 * @brief Specify a container range with an Offset and Length
 *
 * An Extent is a range in a container, specified by Offset into the container
 * and Extent length.
 *
 * This class helps with checking overlaps and bounds and merging of Extents.
 */
class Extent : public Serializeable {
  public:
    Extent() = default;

    /**
     * Constructor
     *
     * @param offset Where in the container does the extent start
     * @param length How long is the extent
     */
    Extent(std::size_t offset, std::size_t length);

    virtual ~Extent() = default;

    /**
     * Is the extent empty - or of length zero
     * @return Whether the extent has zero length.
     */
    bool empty() const;

    /**
     * Does this extent end after the input extent
     * @param ext the input extent
     * @return True if this extent ends after the input one
     */
    bool ends_after(const Extent& ext) const;

    /**
     * The container offset of the end of the extent - i.e. its offset + length
     * - 1;
     * @return The container offset of the end of the extent
     */
    std::size_t get_end() const;

    /**
     * An extent starting at the end of the this one and ending at the start of
     * the input one
     * @return An extent starting at the end of the this one and ending at the start of the input one
     */
    Extent get_right_split(const Extent& ext) const;

    /**
     * An extent starting at the end of the input one and ending at the start of
     * this one
     * @return An extent starting at the end of the input one and ending at the start of this one
     */
    Extent get_left_split(const Extent& ext) const;

    Extent get_right_remainder(const Extent& ext) const;

    std::size_t get_overlapping_length(const Extent& ext) const;

    /**
     * Do both extents have the same offset
     * @return True if both extents have the same offset
     */
    bool has_same_offset(const Extent& ext) const;

    /**
     * Does this extent include the input extent - true if the input offset is
     * g.e. than the offset and the input end offset is l.e. the end offset.
     * @return True if this extent includes in the input extent
     */
    bool includes(const Extent& ext) const;

    bool includes_or_overlaps(const Extent& ext) const;

    bool joined_to_start_of(const Extent& ext) const;

    bool overlaps(const Extent& ext) const;

    std::string to_string() const;

    static Extent get_full_range_extent();

    static std::size_t get_right_most_offset(
        const Extent& ext0, const Extent& ext1);

    static std::size_t get_left_most_offset(
        const Extent& ext0, const Extent& ext1);

    static std::size_t get_right_most_end(
        const Extent& ext0, const Extent& ext1);

    static std::size_t get_left_most_end(
        const Extent& ext0, const Extent& ext1);

    void serialize(
        Dictionary& dict, Format format = Format::FULL) const override;

    void deserialize(
        const Dictionary& dict, Format format = Format::FULL) override;

    bool operator<(const Extent& other) const
    {
        return m_offset < other.m_offset;
    }

    bool operator==(const Extent& other) const
    {
        return m_offset == other.m_offset && m_length == other.m_length;
    }

    friend std::ostream& operator<<(std::ostream& os, Extent const& value)
    {
        os << value.to_string();
        return os;
    }

    std::size_t m_offset{0};
    std::size_t m_length{0};
};
}  // namespace hestia