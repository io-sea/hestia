#include "DaemonManager.h"

#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

DaemonManager::DaemonManager(const std::string& pid_filename)
{
    if (!pid_filename.empty()) {
        m_pid_filename = pid_filename;
    }
}

DaemonManager::Status DaemonManager::start()
{
    const auto pid = ::fork();
    if (pid < 0) {
        m_status = Status::EXIT_FAILED;
        return m_status;
    }
    if (pid > 0) {
        m_status = Status::EXIT_OK;
        return m_status;
    }

    ::umask(0);
    ::chdir("/");
    const auto sid = ::setsid();
    if (sid < 0) {
        m_status = Status::EXIT_FAILED;
        return m_status;
    }

    std::ofstream pid_file(
        m_pid_filename, std::ofstream::out | std::ofstream::trunc);
    pid_file << sid;
    pid_file.close();
    return {};
}

DaemonManager::Status DaemonManager::stop()
{
    pid_t pid;
    std::ifstream pid_file(m_pid_filename);
    if (pid_file.good()) {
        pid_file >> pid;
        ::remove(m_pid_filename.c_str());
    }
    else {
        std::cerr
            << "Error stopping the server daemon. Is the server daemon running?\n";
        m_status = Status::EXIT_FAILED;
        return m_status;
    }

    if (auto rc = ::kill(pid, SIGTERM); rc < 0) {
        std::cerr
            << "Error stopping the server daemon. terminate signal unsuccessful";
    }
    return {};
}