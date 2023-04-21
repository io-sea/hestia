#include "CompositeLayer.h"

int CompositeLayer::add_extent(
    const ostk::Extent& extent_in, bool write, bool overwrite)
{
    auto extents = getExtents(write);
    if (auto iter = extents->find(extentIn.mOffset); iter != extents->end()) {
        if (overwrite || iter->second.mMarkedForDelete) {
            iter->second = extentIn;
        }
    }
    else {
        (*extents)[extentIn.mOffset] = extentIn;
    }
    return 0;
}

int CompositeLayer::add_merge_read_extent(const ostk::Extent& extent_in)
{
    ostk::Extent extent_merge;
    const auto match_code = matchExtent(
        extentIn, &extent_merge, ExtentMatchType::EMT_MERGE, false, true);

    int rc = 0;
    switch (match_code) {
        case ExtentMatchCode::EM_NONE:
            rc = addExtent(extentIn, false, false);
            break;
        case ExtentMatchCode::EM_FULL:
            rc = 0;
            break;
        case ExtentMatchCode::EM_PARTIAL:
            rc = addExtent(extent_merge, false, true);
            break;
        default:
            rc = -EINVAL;
    }
    return rc;
}

int CompositeLayer::mark_for_deletion(const ostk::Extent& extent_in, bool write)
{
    auto extents = getExtents(write);
    if (auto iter = extents->find(extentIn.mOffset); iter != extents->end()) {
        iter->second.mMarkedForDelete = true;
    }
    return 0;
}

void CompositeLayer::delete_marked_extents(bool is_write)
{
    auto extents = getExtents(isWrite);
    auto iter    = extents->begin();
    for (; iter != extents->end();) {
        if (iter->second.mMarkedForDelete) {
            iter = extents->erase(iter);
        }
        else {
            ++iter;
        }
    }
}

std::string CompositeLayer::dump_extents(bool details, bool isWrite)
{
    auto extents = getExtents(isWrite);

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

    auto extents = getExtents(isWrite);

    for (const auto& entry : *extents) {
        remaining_extent++;
        const auto list_extent = &entry.second;

        if (!list_extent->includes(extentIn)) {
            continue;
        }

        if (list_extent->hasSameOffset(extentIn)) {
            if (auto rc = markForDeletion(*list_extent, isWrite); rc) {
                return rc;
            }
            remaining_extent--;

            if (list_extent->endsAfter(extentIn)) {
                const auto right_split = list_extent->getRightSplit(extentIn);
                if (auto rc = addExtent(right_split, isWrite, false); rc) {
                    deleteMarkedExtents(isWrite);
                    return rc;
                }
                remaining_extent++;
            }
            continue;
        }

        const auto left_split = list_extent->getLeftSplit(extentIn);
        if (auto rc = addExtent(left_split, isWrite, true); rc) {
            deleteMarkedExtents(isWrite);
            return rc;
        }
        remaining_extent++;

        if (list_extent->endsAfter(extentIn)) {
            const auto right_remainder =
                list_extent->getRightRemainder(extentIn);
            if (auto rc = addExtent(right_remainder, isWrite, false); rc) {
                deleteMarkedExtents(isWrite);
                return rc;
            }
            remaining_extent++;
        }
    }
    delete_marked_extents(is_write);
    *layer_empty = (remaining_extent == 0);
    return 0;
}

CompositeLayer::ExtentList* CompositeLayer::get_extents(bool isWrite)
{
    auto extents = &m_write_extents;
    if (!isWrite) {
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
    auto extents = getExtents(isWrite);

    bool is_merged = false;
    for (const auto& entry : *extents) {
        const auto list_extent = &entry.second;

        if (list_extent->includes(extentIn)) {
            if (mode == ExtentMatchType::EMT_INTERSECT) {
                *match = extentIn;
            }
            else {
                match->mOffset = list_extent->mOffset;
                match->mLength = list_extent->mLength;
            }
            deleteMarkedExtents(isWrite);
            return ExtentMatchCode::EM_FULL;
        }

        if (list_extent->overlaps(extentIn)) {
            if (mode == ExtentMatchType::EMT_INTERSECT) {
                match->mOffset =
                    ostk::Extent::getRightMostOffset(extentIn, *list_extent);
                auto left_end =
                    ostk::Extent::getLeftMostEnd(extentIn, *list_extent) - 1;
                match->mLength = (left_end - match->mOffset) + 1;
                deleteMarkedExtents(isWrite);
                return ExtentMatchCode::EM_PARTIAL;
            }
            else {
                if (!is_merged) {
                    match->mOffset =
                        ostk::Extent::getLeftMostOffset(extentIn, *list_extent);
                }

                const auto right_end =
                    ostk::Extent::getRightMostEnd(extentIn, *list_extent) - 1;
                match->mLength = (right_end - match->mOffset) + 1;

                if (deletePrevious && list_extent->hasSameOffset(*match)) {
                    markForDeletion(*list_extent, isWrite);
                }

                is_merged = true;
                continue;
            }
        }

        if (mode == ExtentMatchType::EMT_MERGE) {
            if (list_extent->joinedToStartOf(extentIn)) {
                match->mOffset = list_extent->mOffset;
                match->mLength = list_extent->mLength + extentIn.mLength;
                is_merged      = true;
            }
            else if (extentIn.joinedToStartOf(*list_extent)) {
                if (!is_merged) {
                    match->mOffset = extentIn.mOffset;
                    match->mLength = list_extent->mLength + extentIn.mLength;
                    is_merged      = true;
                }
                else {
                    match->mLength += list_extent->mLength;
                }
                if (deletePrevious) {
                    markForDeletion(*list_extent, isWrite);
                }
            }
        }
    };

    delete_marked_extents(is_write);
    return is_merged ? ExtentMatchCode::EM_PARTIAL : ExtentMatchCode::EM_NONE;
}