#include "MockS3Server.h"

namespace hestia::mock {

Server::S3Response Server::get(const std::string& key) const
{
    S3Response response;
    if (auto iter = m_db.find(key); iter != m_db.end()) {
        response.m_body = iter->second;
        return response;
    }
    response.m_status = S3Status::NOT_FOUND;
    return response;
}

Server::S3Response Server::put(
    const std::string& key, const std::vector<char>& data)
{
    m_db[key] = data;
    return {};
}

Server::S3Response Server::remove(const std::string& key)
{
    if (auto iter = m_db.find(key); iter != m_db.end()) {
        m_db.erase(iter);
        return {};
    }

    S3Response response;
    response.m_status = S3Status::NOT_FOUND;
    return response;
}
}  // namespace hestia::mock