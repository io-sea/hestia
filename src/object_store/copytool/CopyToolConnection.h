#pragma once

#include "CopyTool.h"
#include "CopyToolRequest.h"
#include "CopyToolResponse.h"

#include <memory>

class CopyToolConnection {
  public:
    enum class Type { IN_PROCESS, MANAGED, DAEMON };

    enum class State { UNINITIALIZED, INITIALIZED, CONNECTED, ERROR };

    ~CopyToolConnection();

    CopyToolResponse::Ptr makeRequest(const CopyToolRequest& request);

    State connect();

    State disconnect();

    Type getType() const;

    State getState() const;

    State initialize(const CopyToolConfig& config, Type connectionType);

  private:
    void launchExternal();
    void terminateExternal();

    State mState{State::UNINITIALIZED};
    Type mType{Type::IN_PROCESS};
    std::unique_ptr<CopyTool> mInProcessCopyTool;
};