#pragma once

#include "CopyToolConnection.h"

#include <vector>

class CopyToolInterface
{
public:
    bool hasConnection() const;

    HsmObjectStoreResponse::Ptr makeRequest(const HsmObjectStoreRequest& request);

    void initialize(const CopyToolConfig& config, CopyToolConnection::Type connectionType = CopyToolConnection::Type::IN_PROCESS);

private:
    std::unique_ptr<CopyToolConnection> mConnection;
};