#include "CompositeLayer.h"

int CompositeLayer::add_extent(
    const ostk::Extent& extent_in, bool write, bool overwrite)
{
    auto extents = get_extents(write);
    if (auto iter = extents->find(extent_in.mOffset); iter != extents->end()) {
        if (overwrite || iter->second.m_marked_for_delete) {
            iter->second = extent_in;
        }
    }
    else {
        (*extents)[extent_in.mOffset] = extent_in;
    }
    return 0;
}

int CompositeLayer::add_merge_read_extent(const ostk::Extent& extent_in)
{
    ostk::Extent extent_merge;
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
        default:
            rc = -EINVAL;
    }
    return rc;
}

int CompositeLayer::mark_for_deletion(const ostk::Extent& extent_in, bool write)
{
    auto extents = get_extents(write);
    if (auto iter = extents->find(extent_in.mOffset); iter != extents->end()) {
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
    auto extents = get_extents(is_write);

    std::string out;
    for (const auto& list_extent : *extents) {
        out += list_extent.second.toString() + "\n";
    }
    return out;
}

int CompositeLayer::extent_substract(
    const ostk::Extent& extent_in, bool is_write, bool* layer_empty)
{
    int rc               = 0;
    int remaining_extent = 0;

    auto extents = get_extents(is_write);

    for (const auto& entry : *extents) {
        remaining_extent++;
        const auto list_extent = &entry.second;

        if (!list_extent->includes(extent_in)) {
            continue;
        }

        if (list_extent->hasSameOffset(extent_in)) {
            if (auto rc = mark_for_deletion(*list_extent, is_write); rc) {
                return rc;
            }
            remaining_extent--;

            if (list_extent->endsAfter(extent_in)) {
                const auto right_split = list_extent->getRightSplit(extent_in);
                if (auto rc = add_extent(right_split, is_write, false); rc) {
                    delete_marked_extents(is_write);
                    return rc;
                }
                remaining_extent++;
            }
            continue;
        }

        const auto left_split = list_extent->getLeftSplit(extent_in);
        if (auto rc = add_extent(left_split, is_write, true); rc) {
            delete_marked_extents(is_write);
            return rc;
        }
        remaining_extent++;

        if (list_extent->endsAfter(extent_in)) {
            const auto right_remainder =
                list_extent->getRightRemainder(extent_in);
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
    const ostk::Extent& extent_in,
    ostk::Extent* match,
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
                match->mOffset = list_extent->mOffset;
                match->mLength = list_extent->mLength;
            }
            delete_marked_extents(is_write);
            return ExtentMatchCode::EM_FULL;
        }

        if (list_extent->overlaps(extent_in)) {
            if (mode == ExtentMatchType::EMT_INTERSECT) {
                match->mOffset =
                    ostk::Extent::getRightMostOffset(extent_in, *list_extent);
                auto left_end =
                    ostk::Extent::getLeftMostEnd(extent_in, *list_extent) - 1;
                match->mLength = (left_end - match->mOffset) + 1;
                delete_marked_extents(is_write);
                return ExtentMatchCode::EM_PARTIAL;
            }
            else {
                if (!is_merged) {
                    match->mOffset = ostk::Extent::getLeftMostOffset(
                        extent_in, *list_extent);
                }

                const auto right_end =
                    ostk::Extent::getRightMostEnd(extent_in, *list_extent) - 1;
                match->mLength = (right_end - match->mOffset) + 1;

                if (delete_previous && list_extent->hasSameOffset(*match)) {
                    mark_for_deletion(*list_extent, is_write);
                }

                is_merged = true;
                continue;
            }
        }

        if (mode == ExtentMatchType::EMT_MERGE) {
            if (list_extent->joinedToStartOf(extent_in)) {
                match->mOffset = list_extent->mOffset;
                match->mLength = list_extent->mLength + extent_in.mLength;
                is_merged      = true;
            }
            else if (extent_in.joinedToStartOf(*list_extent)) {
                if (!is_merged) {
                    match->mOffset = extent_in.mOffset;
                    match->mLength = list_extent->mLength + extent_in.mLength;
                    is_merged      = true;
                }
                else {
                    match->mLength += list_extent->mLength;
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