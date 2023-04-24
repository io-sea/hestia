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

    CopyToolResponse::Ptr make_request(const CopyToolRequest& request);

    State connect();

    State disconnect();

    Type get_type() const;

    State get_state() const;

    State initialize(const CopyToolConfig& config, Type connection_type);

  private:
    void launch_external();
    void terminate_external();

    State m_state{State::UNINITIALIZED};
    Type m_type{Type::IN_PROCESS};
    std::unique_ptr<CopyTool> m_in_process_copy_tool;
};