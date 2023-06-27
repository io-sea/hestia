#include "CompositeLayer.h"

namespace hestia {
int CompositeLayer::add_extent(
    const Extent& extent_in, bool write, bool overwrite)
{
    auto extents = get_extents(write);
    if (auto iter = extents->find(extent_in.m_offset); iter != extents->end()) {
        if (overwrite || iter->second.m_marked_for_delete) {
            iter->second = extent_in;
        }
    }
    else {
        (*extents)[extent_in.m_offset] = extent_in;
    }
    return 0;
}

int CompositeLayer::add_merge_read_extent(const Extent& extent_in)
{
    Extent extent_merge;
    const auto match_code = match_extent(
        extent_in, &extent_merge, ExtentMatchType::EMT_MERGE, false, true);

    int rc = 0;
    switch (match_code) {
        case ExtentMatchCode::EM_NONE:
            rc = add_extent(extent_in, false, false);
            break;
        case ExtentMatchCode::EM_FULL:
            rc = 0;
            break;
        case ExtentMatchCode::EM_PARTIAL:
            rc = add_extent(extent_merge, false, true);
            break;
        case ExtentMatchCode::EM_ERROR:
        default:
            rc = -EINVAL;
    }
    return rc;
}

int CompositeLayer::mark_for_deletion(const Extent& extent_in, bool write)
{
    auto extents = get_extents(write);
    if (auto iter = extents->find(extent_in.m_offset); iter != extents->end()) {
        iter->second.m_marked_for_delete = true;
    }
    return 0;
}

void CompositeLayer::delete_marked_extents(bool is_write)
{
    auto extents = get_extents(is_write);
    auto iter    = extents->begin();
    for (; iter != extents->end();) {
        if (iter->second.m_marked_for_delete) {
            iter = extents->erase(iter);
        }
        else {
            ++iter;
        }
    }
}

std::string CompositeLayer::dump_extents(bool details, bool is_write)
{
    (void)details;

    auto extents = get_extents(is_write);

    std::string out;
    for (const auto& list_extent : *extents) {
        out += list_extent.second.to_string() + "\n";
    }
    return out;
}

int CompositeLayer::extent_substract(
    const Extent& extent_in, bool is_write, bool* layer_empty)
{
    int remaining_extent = 0;

    auto extents = get_extents(is_write);

    for (const auto& entry : *extents) {
        remaining_extent++;
        const auto list_extent = &entry.second;

        if (!list_extent->includes(extent_in)) {
            continue;
        }

        if (list_extent->has_same_offset(extent_in)) {
            if (auto rc = mark_for_deletion(*list_extent, is_write); rc) {
                return rc;
            }
            remaining_extent--;

            if (list_extent->ends_after(extent_in)) {
                const auto right_split =
                    list_extent->get_right_split(extent_in);
                if (auto rc = add_extent(right_split, is_write, false); rc) {
                    delete_marked_extents(is_write);
                    return rc;
                }
                remaining_extent++;
            }
            continue;
        }

        const auto left_split = list_extent->get_left_split(extent_in);
        if (auto rc = add_extent(left_split, is_write, true); rc) {
            delete_marked_extents(is_write);
            return rc;
        }
        remaining_extent++;

        if (list_extent->ends_after(extent_in)) {
            const auto right_remainder =
                list_extent->get_right_remainder(extent_in);
            if (auto rc = add_extent(right_remainder, is_write, false); rc) {
                delete_marked_extents(is_write);
                return rc;
            }
            remaining_extent++;
        }
    }
    delete_marked_extents(is_write);
    *layer_empty = (remaining_extent == 0);
    return 0;
}

CompositeLayer::ExtentList* CompositeLayer::get_extents(bool is_write)
{
    auto extents = &m_write_extents;
    if (!is_write) {
        extents = &m_read_extents;
    }
    return extents;
}

bool CompositeLayer::has_write_extents() const
{
    return !m_write_extents.empty();
}

bool CompositeLayer::has_read_extents() const
{
    return !m_read_extents.empty();
}

ExtentMatchCode CompositeLayer::match_extent(
    const Extent& extent_in,
    Extent* match,
    ExtentMatchType mode,
    bool is_write,
    bool delete_previous)
{
    auto extents = get_extents(is_write);

    bool is_merged = false;
    for (const auto& entry : *extents) {
        const auto list_extent = &entry.second;

        if (list_extent->includes(extent_in)) {
            if (mode == ExtentMatchType::EMT_INTERSECT) {
                *match = extent_in;
            }
            else {
                match->m_offset = list_extent->m_offset;
                match->m_length = list_extent->m_length;
            }
            delete_marked_extents(is_write);
            return ExtentMatchCode::EM_FULL;
        }

        if (list_extent->overlaps(extent_in)) {
            if (mode == ExtentMatchType::EMT_INTERSECT) {
                match->m_offset =
                    Extent::get_right_most_offset(extent_in, *list_extent);
                auto left_end =
                    Extent::get_left_most_end(extent_in, *list_extent) - 1;
                match->m_length = (left_end - match->m_offset) + 1;
                delete_marked_extents(is_write);
                return ExtentMatchCode::EM_PARTIAL;
            }
            else {
                if (!is_merged) {
                    match->m_offset =
                        Extent::get_left_most_offset(extent_in, *list_extent);
                }

                const auto right_end =
                    Extent::get_right_most_end(extent_in, *list_extent) - 1;
                match->m_length = (right_end - match->m_offset) + 1;

                if (delete_previous && list_extent->has_same_offset(*match)) {
                    mark_for_deletion(*list_extent, is_write);
                }

                is_merged = true;
                continue;
            }
        }

        if (mode == ExtentMatchType::EMT_MERGE) {
            if (list_extent->joined_to_start_of(extent_in)) {
                match->m_offset = list_extent->m_offset;
                match->m_length = list_extent->m_length + extent_in.m_length;
                is_merged       = true;
            }
            else if (extent_in.joined_to_start_of(*list_extent)) {
                if (!is_merged) {
                    match->m_offset = extent_in.m_offset;
                    match->m_length =
                        list_extent->m_length + extent_in.m_length;
                    is_merged = true;
                }
                else {
                    match->m_length += list_extent->m_length;
                }
                if (delete_previous) {
                    mark_for_deletion(*list_extent, is_write);
                }
            }
        }
    };

    delete_marked_extents(is_write);
    return is_merged ? ExtentMatchCode::EM_PARTIAL : ExtentMatchCode::EM_NONE;
}
}  // namespace hestia