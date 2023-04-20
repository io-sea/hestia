#include "CopyToolRequest.h"

CopyToolRequest::CopyToolRequest(const HsmObjectStoreRequest& request)
    : mObjectStoreRequest(request)
{

}

CopyToolRequest::CopyToolRequest(const std::string& requestStr)
    : mObjectStoreRequest("0", HsmObjectStoreRequestMethod::COPY)
{
    deserialize(requestStr);
}

void CopyToolRequest::deserialize(const std::string& requestStr)
{
    std::stringstream sstr(requestStr);
    std::string line;

/*
    while(std::getline(sstr, line, '\n'))
    {
        if (auto loc = line.find(OBJECT_TOKEN); loc != std::string::npos)
        {
            mObjectStoreRequest.mBaseRequest.mObject.mId = line.substr(loc, line.length() - loc);
        }
        else if (auto loc = line.find(OFFSET_TOKEN); loc != std::string::npos)
        {
            mObjectStoreRequest.mBaseRequest.mExtent.mOffset = std::stoull(line.substr(loc, line.length() - loc));
        }
        else if (auto loc = line.find(LENGTH_TOKEN); loc != std::string::npos)
        {
            mObjectStoreRequest.mBaseRequest.mExtent.mLength = std::stoull(line.substr(loc, line.length() - loc));
        }
        else if (auto loc = line.find(SOURCE_TOKEN); loc != std::string::npos)
        {
            mObjectStoreRequest.mSourceTier = std::stoi(line.substr(loc, line.length() - loc));
        }
        else if (auto loc = line.find(TARGET_TOKEN); loc != std::string::npos)
        {
            mObjectStoreRequest.mTargetTier = std::stoi(line.substr(loc, line.length() - loc));
        }
    }
*/
}

std::string CopyToolRequest::serialize() const
{
    std::stringstream sstr;
    sstr << OBJECT_TOKEN << mObjectStoreRequest.object().id() << "\n";
    sstr << OFFSET_TOKEN << mObjectStoreRequest.extent().mOffset << "\n";
    sstr << LENGTH_TOKEN << mObjectStoreRequest.extent().mLength << "\n";
    sstr << SOURCE_TOKEN << mObjectStoreRequest.sourceTier() << "\n";
    sstr << TARGET_TOKEN << mObjectStoreRequest.targetTier() << "\n";
    return sstr.str();
}

const HsmObjectStoreRequest& CopyToolRequest::getObjectStoreRequest() const 
{
    return mObjectStoreRequest;
}
