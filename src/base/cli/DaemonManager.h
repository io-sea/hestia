#pragma once

#include <string>

class DaemonManager {
  public:
    enum class Status { OK, EXIT_OK, EXIT_FAILED };

    DaemonManager(const std::string& pid_filename = {});

    Status start();

    Status stop();

  private:
    Status m_status{Status::OK};
    std::string m_pid_filename{"/tmp/deimos_pid"};
};