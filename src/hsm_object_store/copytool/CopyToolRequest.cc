#include "CopyToolRequest.h"

CopyToolRequest::CopyToolRequest(const HsmObjectStoreRequest& request) :
    m_object_store_request(request)
{
}

CopyToolRequest::CopyToolRequest(const std::string& request_str) :
    m_object_store_request("0", HsmObjectStoreRequestMethod::COPY)
{
    deserialize(request_str);
}

void CopyToolRequest::deserialize(const std::string& request_str)
{
    std::stringstream sstr(request_str);
    std::string line;

    /*
        while(std::getline(sstr, line, '\n'))
        {
            if (auto loc = line.find(OBJECT_TOKEN); loc != std::string::npos)
            {
                mObjectStoreRequest.mBaseRequest.mObject.mId = line.substr(loc,
       line.length() - loc);
            }
            else if (auto loc = line.find(OFFSET_TOKEN); loc !=
       std::string::npos)
            {
                mObjectStoreRequest.mBaseRequest.mExtent.mOffset =
       std::stoull(line.substr(loc, line.length() - loc));
            }
            else if (auto loc = line.find(LENGTH_TOKEN); loc !=
       std::string::npos)
            {
                mObjectStoreRequest.mBaseRequest.mExtent.mLength =
       std::stoull(line.substr(loc, line.length() - loc));
            }
            else if (auto loc = line.find(SOURCE_TOKEN); loc !=
       std::string::npos)
            {
                mObjectStoreRequest.mSourceTier = std::stoi(line.substr(loc,
       line.length() - loc));
            }
            else if (auto loc = line.find(TARGET_TOKEN); loc !=
       std::string::npos)
            {
                mObjectStoreRequest.mTargetTier = std::stoi(line.substr(loc,
       line.length() - loc));
            }
        }
    */
}

std::string CopyToolRequest::serialize() const
{
    std::stringstream sstr;
    sstr << object_token << m_object_store_request.object().id() << "\n";
    sstr << offset_token << m_object_store_request.extent().m_offset << "\n";
    sstr << length_token << m_object_store_request.extent().m_length << "\n";
    sstr << source_token << m_object_store_request.source_tier() << "\n";
    sstr << target_token << m_object_store_request.target_tier() << "\n";
    return sstr.str();
}

const HsmObjectStoreRequest& CopyToolRequest::get_object_store_request() const
{
    return m_object_store_request;
}
