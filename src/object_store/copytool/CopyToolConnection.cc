#include "CopyToolConnection.h"

#include "HsmObjectStoreClientManager.h"

CopyToolConnection::~CopyToolConnection()
{
    if (m_type == Type::MANAGED) {
        terminate_external();
    }
    else if (m_type == Type::DAEMON) {
        disconnect();
    }
}

CopyToolConnection::State CopyToolConnection::initialize(
    const CopyToolConfig& config, Type connection_type)
{
    m_type = connection_type;

    if (m_type == Type::IN_PROCESS) {
        mInProcessCopyTool = std::make_unique<CopyTool>(nullptr);
        // mInProcessCopyTool->initialize(config);
        m_state = State::CONNECTED;
    }
    else if (m_type == Type::MANAGED) {
        launch_external();
    }
    else {
        m_state = State::INITIALIZED;
    }
    return m_state;
}

CopyToolConnection::State CopyToolConnection::connect()
{
    if (m_state != State::INITIALIZED) {
        return m_state;
    }
    return m_state;
}

CopyToolConnection::State CopyToolConnection::disconnect()
{
    if (m_state != State::CONNECTED) {
        return m_state;
    }
    return m_state;
}

CopyToolResponse::Ptr CopyToolConnection::make_request(
    const CopyToolRequest& request)
{
    auto response = CopyToolResponse::Create();

    if (m_state != State::CONNECTED) {
        response->setError(
            {CopyToolError::Code::ERROR_NO_CONNECTION,
             "Attempted copy with no connection"});
        return response;
    }

    if (m_type == Type::IN_PROCESS) {
        response->setObjectStoreRespose(
            mInProcessCopyTool->makeObjectStoreRequest(
                request.getObjectStoreRequest()));
        return response;
    }

    response->setError(
        {CopyToolError::Code::ERROR_NO_CONNECTION,
         "Attempted copy with no connection"});
    return response;
}

CopyToolConnection::Type CopyToolConnection::get_type() const
{
    return m_type;
}

CopyToolConnection::State CopyToolConnection::get_state() const
{
    return m_state;
}

void CopyToolConnection::launch_external() {}

void CopyToolConnection::terminate_external() {}