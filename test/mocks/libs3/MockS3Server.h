#pragma once

#include <map>
#include <string>
#include <vector>

namespace hestia::mock {

class Server {
  public:
    enum class S3Status { OK, NOT_FOUND };

    struct S3Response {
        S3Status m_status{S3Status::OK};
        std::vector<char> m_body;
    };

    S3Response get(const std::string& key) const;

    S3Response put(const std::string& key, const std::vector<char>& data);

    S3Response remove(const std::string& key);

  private:
    std::map<std::string, std::vector<char>> m_db;
};
}  // namespace hestia::mock