#include "CopyToolWebClient.h"

#include "CurlClient.h"

namespace hestia {
CopyToolWebClient::CopyToolWebClient(const std::string& endpoint) :
    m_end_point(endpoint),
    m_http_client(std::make_unique<CurlClient>(CurlClientConfig()))
{
}
}  // namespace hestia