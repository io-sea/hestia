#pragma once

#include <ostk/Extent.h>
#include <ostk/Uuid.h>

#include <map>

enum class ExtentMatchCode { EM_ERROR, EM_NONE, EM_PARTIAL, EM_FULL };

enum class ExtentMatchType { EMT_INTERSECT, EMT_MERGE };

class MarkableExtent : public ostk::Extent {
  public:
    MarkableExtent() : ostk::Extent() {}

    MarkableExtent(std::size_t offset, std::size_t length) : ostk::Extent() {}

    MarkableExtent(const ostk::Extent& extent) : ostk::Extent(extent) {}
    bool m_marked_for_delete{false};
};

class CompositeLayer {
  public:
    int add_extent(
        const ostk::Extent& extent_in, bool is_write, bool overwrite);

    int add_merge_read_extent(const ostk::Extent& extent_in);

    void delete_marked_extents(bool is_write);

    std::string dump_extents(bool details, bool is_write);

    int extent_substract(
        const ostk::Extent& extent_in, bool is_write, bool* layer_empty);

    using ExtentList = std::map<std::size_t, MarkableExtent>;
    ExtentList* get_extents(bool is_write);

    bool has_read_extents() const;

    bool has_write_extents() const;

    int mark_for_deletion(const ostk::Extent& extent_in, bool is_write);

    ExtentMatchCode match_extent(
        const ostk::Extent& extent_in,
        ostk::Extent* match,
        ExtentMatchType mode,
        bool is_write,
        bool delete_previous);

    bool operator<(const CompositeLayer& other) const
    {
        return m_priority < other.m_priority;
    }

    ostk::Uuid m_id;
    ostk::Uuid m_parent_id;
    uint32_t m_priority{0};

  private:
    ExtentList m_read_extents;
    ExtentList m_write_extents;
};