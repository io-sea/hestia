#pragma once

#include "HttpClient.h"

#include <memory>
#include <string>

namespace hestia {
class HttpClient;

class CopyToolWebClient {
  public:
    CopyToolWebClient(const std::string& endpoint);

  private:
    std::string m_end_point;
    std::unique_ptr<HttpClient> m_http_client;
};
}  // namespace hestia