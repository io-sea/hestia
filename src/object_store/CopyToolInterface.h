#pragma once

#include "CopyToolConnection.h"

#include <vector>

class CopyToolInterface {
  public:
    bool has_connection() const;

    HsmObjectStoreResponse::Ptr make_request(
        const HsmObjectStoreRequest& request);

    void initialize(
        const CopyToolConfig& config,
        CopyToolConnection::Type connection_type =
            CopyToolConnection::Type::IN_PROCESS);

  private:
    std::unique_ptr<CopyToolConnection> m_connection;
};