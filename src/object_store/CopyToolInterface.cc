#include "CopyToolInterface.h"

void CopyToolInterface::initialize(const CopyToolConfig& config, CopyToolConnection::Type connectionType)
{
    if (mConnection)
    {
        return;
    }

    mConnection = std::make_unique<CopyToolConnection>();
    mConnection->initialize(config, connectionType);
    mConnection->connect();
}

bool CopyToolInterface::hasConnection() const
{
    return mConnection && mConnection->getState() == CopyToolConnection::State::CONNECTED;
}

HsmObjectStoreResponse::Ptr CopyToolInterface::makeRequest(const HsmObjectStoreRequest& request)
{
    if (!hasConnection())
    {
        const std::string msg = "Attempted copytool operation with no connection";
        auto copytool_response = CopyToolResponse::Create();
        copytool_response->setError({CopyToolError::Code::ERROR_NO_CONNECTION, msg});
        return HsmObjectStoreResponse::Create(request, std::move(copytool_response));
    }

    CopyToolRequest copytool_request(request);
    auto copytool_response = mConnection->makeRequest(copytool_request);
    return HsmObjectStoreResponse::Create(request, std::move(copytool_response));
}
