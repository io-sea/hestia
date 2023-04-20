#pragma once

#include <ostk/Extent.h>
#include <ostk/Uuid.h>

#include <map>

enum class ExtentMatchCode
{
    EM_ERROR,
    EM_NONE,
    EM_PARTIAL,
    EM_FULL
};

enum class ExtentMatchType
{
    EMT_INTERSECT,
    EMT_MERGE
};

class MarkableExtent : public ostk::Extent
{
public:
    MarkableExtent()
        : ostk::Extent()
    {

    }

    MarkableExtent(std::size_t offset, std::size_t length)
        : ostk::Extent()
    {

    }

    MarkableExtent(const ostk::Extent& extent)
        : ostk::Extent(extent)
    {
    }
    bool mMarkedForDelete{false};
};

class CompositeLayer
{
public:
    int addExtent(const ostk::Extent& extentIn, bool isWrite, bool overwrite);

    int addMergeReadExtent(const ostk::Extent& extentIn);

    void deleteMarkedExtents(bool isWrite);

    std::string dumpExtents(bool details, bool isWrite);

    int extentSubstract(const ostk::Extent& extentIn, bool isWrite, bool* layerEmpty);

    using ExtentList = std::map<std::size_t, MarkableExtent>;
    ExtentList* getExtents(bool isWrite);

    bool hasReadExtents() const;

    bool hasWriteExtents() const;

    int markForDeletion(const ostk::Extent& extentIn, bool isWrite);

    ExtentMatchCode matchExtent(const ostk::Extent& extentIn, ostk::Extent* match, ExtentMatchType mode, bool isWrite, bool deletePrevious);

    bool operator<(const CompositeLayer& other) const
    {
        return mPriority < other.mPriority;
    }

    ostk::Uuid mId;
    ostk::Uuid mParentId;
    uint32_t mPriority{0};

private:
    ExtentList mReadExtents;
    ExtentList mWriteExtents;
};