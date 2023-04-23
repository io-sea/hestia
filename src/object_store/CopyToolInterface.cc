#include "CopyToolInterface.h"

void CopyToolInterface::initialize(
    const CopyToolConfig& config, CopyToolConnection::Type connection_type)
{
    if (m_connection) {
        return;
    }

    m_connection = std::make_unique<CopyToolConnection>();
    m_connection->initialize(config, connection_type);
    m_connection->connect();
}

bool CopyToolInterface::has_connection() const
{
    return m_connection
           && m_connection->get_state() == CopyToolConnection::State::CONNECTED;
}

HsmObjectStoreResponse::Ptr CopyToolInterface::make_request(
    const HsmObjectStoreRequest& request)
{
    if (!has_connection()) {
        const std::string msg =
            "Attempted copytool operation with no connection";
        auto copytool_response = CopyToolResponse::create();
        copytool_response->set_error(
            {CopyToolError::Code::ERROR_NO_CONNECTION, msg});
        return HsmObjectStoreResponse::create(
            request, std::move(copytool_response));
    }

    CopyToolRequest copytool_request(request);
    auto copytool_response = m_connection->make_request(copytool_request);
    return HsmObjectStoreResponse::create(
        request, std::move(copytool_response));
}
