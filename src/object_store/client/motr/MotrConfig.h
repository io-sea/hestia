#pragma once

#include <string>
#include <vector>

struct MotrHsmTierInfo
{
    std::string mName;
    std::string mIdentifier;
};

struct MotrConfig
{
    std::string mLocalAddress;
    std::string mHaAddress;
    std::string mProfile;
    std::string mProcFid;
    std::string mHsmConfigPath;
    std::vector<MotrHsmTierInfo> mTierInfo;
};