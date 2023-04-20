#include "CopyToolConnection.h"

#include "HsmObjectStoreClientManager.h"

CopyToolConnection::~CopyToolConnection()
{
    if (mType == Type::MANAGED) {
        terminateExternal();
    }
    else if (mType == Type::DAEMON) {
        disconnect();
    }
}

CopyToolConnection::State CopyToolConnection::initialize(
    const CopyToolConfig& config, Type connectionType)
{
    mType = connectionType;

    if (mType == Type::IN_PROCESS) {
        mInProcessCopyTool = std::make_unique<CopyTool>(nullptr);
        // mInProcessCopyTool->initialize(config);
        mState = State::CONNECTED;
    }
    else if (mType == Type::MANAGED) {
        launchExternal();
    }
    else {
        mState = State::INITIALIZED;
    }
    return mState;
}

CopyToolConnection::State CopyToolConnection::connect()
{
    if (mState != State::INITIALIZED) {
        return mState;
    }
    return mState;
}

CopyToolConnection::State CopyToolConnection::disconnect()
{
    if (mState != State::CONNECTED) {
        return mState;
    }
    return mState;
}

CopyToolResponse::Ptr CopyToolConnection::makeRequest(
    const CopyToolRequest& request)
{
    auto response = CopyToolResponse::Create();

    if (mState != State::CONNECTED) {
        response->setError(
            {CopyToolError::Code::ERROR_NO_CONNECTION,
             "Attempted copy with no connection"});
        return response;
    }

    if (mType == Type::IN_PROCESS) {
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

CopyToolConnection::Type CopyToolConnection::getType() const
{
    return mType;
}

CopyToolConnection::State CopyToolConnection::getState() const
{
    return mState;
}

void CopyToolConnection::launchExternal() {}

void CopyToolConnection::terminateExternal() {}